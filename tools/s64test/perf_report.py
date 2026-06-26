#!/usr/bin/env python3
"""SWEET64 performance report.

Measures SWEET64 execution cost using the firmware's per-run counters
(m32S64programCyclesIdx / m32S64programInstrIdx), which the ^T trace loop
accumulates around each executeInstruction call -- so they time the actual
instruction execution, NOT the trace serial output.  Because the debug
monitor is cooperative (it owns the CPU during a run), these counts are clean.

Requires firmware built with useDebugCPUreading + useDebugTerminalSWEET64
(the ATmega2560 debug build).  Reflash after the instrumentation change before
running this.

Each case runs a program dominated by one opcode (repeated N times) so the
average cycles/instruction reflects that opcode; the trailing Done adds one
cheap instruction.  Output is a report, not pass/fail -- save it as a baseline
and diff later to catch performance regressions.

Usage:
    python perf_report.py <port> <baud>
"""

import sys
import serial
from s64terminal import S64Terminal, S64TerminalError
from s64assembler import assemble, S64AssemblerError
from s64registers import set_registers
from s64exec import run_ram_program
from s64variables import read_variables

CYCLES_VAR = 'm32S64programCyclesIdx'
INSTR_VAR = 'm32S64programInstrIdx'

REP = 10        # repetitions of the measured opcode per case


class PerfCase:
    def __init__(self, name, body, inputs=None, reps=REP):
        self.name = name
        self.body = body            # list of lines repeated `reps` times
        self.inputs = inputs or {}  # {reg_number(1..7): value}
        self.reps = reps

    def program(self):
        prog = []
        for _ in range(self.reps):
            prog.extend(self.body)
        prog.append("Done")
        return prog


CASES = [
    PerfCase("LdRegByte", ["LdRegByte 11 2A"]),
    PerfCase("AddByteToX", ["AddByteToX 1 01"]),
    PerfCase("AddYtoX", ["AddYtoX 21"], inputs={2: 0x01}),
    PerfCase("SubByteFromX", ["SubByteFromX 1 01"], inputs={1: 0xFFFFFFFF}),
    PerfCase("ShiftRegLeft", ["ShiftRegLeft 11"], inputs={1: 0x01}),
    PerfCase("Mul2byByte", ["Mul2byByte 02"], inputs={2: 0x01}),
    PerfCase("Mul2by1", ["Mul2by1"], inputs={1: 0x02, 2: 0x01}),
    PerfCase("Div2byByte", ["Div2byByte 02"], inputs={2: 0x100000}),
    PerfCase("Isqrt", ["Isqrt 11"], inputs={1: 0xFFFFFFFF}),
    PerfCase("CmpXtoY", ["CmpXtoY 21"], inputs={1: 0x10, 2: 0x20}),
]


def measure(term, case):
    set_registers(term, 0, [0] * 7)                 # clean baseline
    for reg_number, value in sorted(case.inputs.items()):
        set_registers(term, reg_number - 1, [value])
    assemble(term, 0, case.program())
    run_ram_program(term, 0, max_lines=0)
    _, by_label = read_variables(term)
    cycles = by_label[CYCLES_VAR].hex_value
    instrs = by_label[INSTR_VAR].hex_value
    return cycles, instrs


def main():
    if len(sys.argv) < 3:
        print('Usage: {} <port> <baud>'.format(sys.argv[0]))
        sys.exit(1)
    port, baud = sys.argv[1], int(sys.argv[2])

    with S64Terminal(port, baud) as term:
        term.wait_for_prompt()
        print('{:<16} {:>6} {:>10} {:>12}'.format(
            'OPCODE', 'INSTR', 'CYCLES', 'CYCLES/INSTR'))
        print('-' * 48)
        for case in CASES:
            try:
                cycles, instrs = measure(term, case)
            except (S64AssemblerError, S64TerminalError, KeyError) as e:
                print('{:<16} ERROR: {}'.format(case.name, e))
                continue
            per = (cycles / instrs) if instrs else 0
            print('{:<16} {:>6} {:>10} {:>12.1f}'.format(
                case.name, instrs, cycles, per))
        print()
        print('cycles are timer0 cycles; CYCLES/INSTR includes the trailing Done.')


if __name__ == '__main__':
    main()

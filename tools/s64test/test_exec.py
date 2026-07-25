#!/usr/bin/env python3
"""Exploratory SWEET64 execution test.

Assembles a small program, runs it to completion, and prints the resulting
register state.  Used to confirm the execution loop end to end and to verify
the register-operand nibble -> register-index mapping before locking in
assertions.

Usage:
    python test_exec.py <port> <baud>
"""

import sys
from s64terminal import S64Terminal, S64TerminalError
from s64assembler import S64AssemblerError
from s64exec import run_program


# Register operand nibble rules:
#   - single-register ops (e.g. AddByteToX): low nibble = register, upper 0
#   - two-register / arithmetic ops (LdRegByte, AddYtoX...): high=X, low=Y
# load 0x2A into register 1, then add 0x08 -> expect 0x32 in register 1
PROGRAM = [
    'LdRegByte 11 2A',      # load register (X=1,Y=1) with immediate 0x2A
    'AddByteToX 1 08',      # add 0x08 to register 1 (single-register: low nibble)
    'Done',
]


def main():
    if len(sys.argv) < 3:
        print('Usage: {} <port> <baud>'.format(sys.argv[0]))
        sys.exit(1)

    port = sys.argv[1]
    baud = int(sys.argv[2])

    with S64Terminal(port, baud) as term:
        term.wait_for_prompt()

        print('Program:')
        for line in PROGRAM:
            print('  {}'.format(line))
        print()
        print('Assembling, running to completion, reading registers...')

        try:
            by_index, by_label = run_program(term, PROGRAM)
        except (S64AssemblerError, S64TerminalError) as e:
            print('FAIL: {}'.format(e))
            sys.exit(1)

        print()
        print('{:<5} {:<18} {:<12} {}'.format('IDX', 'VALUE', 'DECIMAL', 'LABEL'))
        print('-' * 60)
        for reg in by_index:
            if reg is None:
                continue
            dec = '-' if reg.dec_value is None else reg.dec_value
            print('0x{:02X}  {:016X}   {:<12} {}'.format(
                reg.index, reg.hex_value, dec, reg.label))

        # register 1 (s64reg64_1) should hold 0x32. Note: the DECIMAL column is
        # a fixed-point view with 3 implied decimal places (value / 1000), so
        # 0x32 displays as 0 with 0 fractional digits selected. hex_value is the
        # authoritative raw register content for assertions.
        reg = by_index[0]
        ok = reg is not None and reg.hex_value == 0x32
        print()
        print('register 0 (s64reg64_1) = 0x{:X} (expected 0x32) -> {}'.format(
            reg.hex_value if reg else None, 'PASS' if ok else 'FAIL'))
        sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()

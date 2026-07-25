#!/usr/bin/env python3
"""Connectivity and instruction-set discovery test for the MPGuino SWEET64
debug terminal.

Usage:
    python test_connectivity.py <port> [baud]
    python test_connectivity.py --mock

Examples:
    python test_connectivity.py COM3 9600
    python test_connectivity.py /dev/ttyUSB0 9600
    python test_connectivity.py --mock
"""

import sys
import serial
from s64terminal import S64Terminal, S64TerminalError
from s64instructions import fetch_instruction_set


def main():
    if len(sys.argv) < 2:
        print('Usage: {} <port> [baud]'.format(sys.argv[0]))
        print('       {} --mock'.format(sys.argv[0]))
        sys.exit(1)

    use_mock = sys.argv[1] == '--mock'

    if use_mock:
        from mock_terminal import MockTerminal
        print('Using mock terminal (no hardware required).')
        term_ctx = MockTerminal()
    else:
        port = sys.argv[1]
        baud = int(sys.argv[2]) if len(sys.argv) > 2 else 9600
        print('Connecting to {} at {} baud...'.format(port, baud))
        term_ctx = S64Terminal(port, baud)

    try:
        with term_ctx as term:

            # --- sync to prompt ---
            print('Waiting for prompt...')
            try:
                term.wait_for_prompt()
            except S64TerminalError as e:
                print('FAIL: could not reach prompt: {}'.format(e))
                sys.exit(1)
            print('OK: monitor is ready')

            # --- fetch instruction set ---
            print('Fetching SWEET64 instruction set via ^I...')
            try:
                by_mnemonic, by_opcode = fetch_instruction_set(term)
            except S64TerminalError as e:
                print('FAIL: error during ^I: {}'.format(e))
                sys.exit(1)

            total   = len(by_opcode)
            valid   = sum(1 for i in by_opcode if i is not None)
            invalid = total - valid

            print('OK: {} opcodes total, {} valid, {} invalid/reserved'.format(
                total, valid, invalid))

            # --- print instruction table ---
            print()
            print('{:<5} {:<5} {:<3} {:<28} {}'.format(
                'IDX', 'FMT', '#OP', 'MNEMONIC', 'OPERANDS'))
            print('-' * 72)
            for instr in by_opcode:
                if instr is None:
                    continue
                print('0x{:02X}  {:04X}  {:<3} {:<28} {}'.format(
                    instr.index,
                    instr.format_word,
                    instr.operand_byte_count,
                    instr.mnemonic,
                    instr.operand_desc,
                ))

            # --- read registers (skip under mock) ---
            if not use_mock:
                print()
                print('Reading SWEET64 registers via ^E...')
                from s64registers import read_registers
                by_index, by_label = read_registers(term)
                print('OK: {} registers read'.format(len(by_index)))
                print()
                print('{:<5} {:<18} {:<12} {}'.format(
                    'IDX', 'VALUE', 'DECIMAL', 'LABEL'))
                print('-' * 60)
                for reg in by_index:
                    if reg is None:
                        continue
                    print('0x{:02X}  {:016X}   {:<12} {}'.format(
                        reg.index, reg.hex_value, reg.dec_value, reg.label))

            print()
            print('Connectivity test PASSED.')

    except S64TerminalError as e:
        print('FAIL: {}'.format(e))
        sys.exit(1)
    except serial.SerialException as e:
        print('FAIL: serial port error: {}'.format(e))
        sys.exit(1)


if __name__ == '__main__':
    main()

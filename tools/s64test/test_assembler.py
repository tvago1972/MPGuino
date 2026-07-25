#!/usr/bin/env python3
"""Exercise the SWEET64 RAM assembler driver.

Assembles a small program into program RAM and prints the monitor's
disassembly echo for each line, then confirms a clean return to the ']'
prompt.  Does not execute the program (that comes next).

Usage:
    python test_assembler.py <port> <baud>
"""

import sys
from s64terminal import S64Terminal, S64TerminalError
from s64assembler import assemble, S64AssemblerError


# A trivial program: load a byte into a register, then return.
# Operands are hex bytes; '12' is a nibble-packed register operand (X=1,Y=2).
PROGRAM = [
    'LdRegByte 12 2A',      # load register with immediate 0x2A
    'Done',                 # return to caller
]


def main():
    if len(sys.argv) < 3:
        print('Usage: {} <port> <baud>'.format(sys.argv[0]))
        sys.exit(1)

    port = sys.argv[1]
    baud = int(sys.argv[2])

    with S64Terminal(port, baud) as term:
        term.wait_for_prompt()
        print('Assembling {} instruction(s) at RAM address 0x00...'.format(
            len(PROGRAM)))
        try:
            echoes = assemble(term, 0, PROGRAM)
        except (S64AssemblerError, S64TerminalError) as e:
            print('FAIL: {}'.format(e))
            sys.exit(1)

        print('--- assembler echo ---')
        for line in echoes:
            print('  {}'.format(line))

        # confirm we are back at the normal prompt and responsive
        term.wait_for_prompt()
        print()
        print('Assembler test PASSED (returned to prompt cleanly).')


if __name__ == '__main__':
    main()

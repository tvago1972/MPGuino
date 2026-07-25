#!/usr/bin/env python3
"""Raw command dumper for the MPGuino SWEET64 debug terminal.

Sends a single command to the monitor and prints the raw response lines,
one per line with repr() so whitespace and separators are visible.  Useful
for inspecting exact output formats while building parsers.

Commands may include control characters using caret notation, e.g.:
    ^E  -> Ctrl+E (0x05)   list SWEET64 registers
    ^I  -> Ctrl+I (0x09)   list SWEET64 instructions
    ^L  -> Ctrl+L (0x0C)   list SWEET64 program code

Usage:
    python raw_command.py <port> <baud> <command>

Examples:
    python raw_command.py COM3 38400 "^E"
    python raw_command.py COM3 38400 "0.5P"
"""

import sys
import re
from s64terminal import S64Terminal, S64TerminalError


def expand_carets(s):
    """Convert caret notation (^A..^Z) to the corresponding control chars."""
    def repl(m):
        return chr(ord(m.group(1)) - ord('@'))
    return re.sub(r'\^([A-Z])', repl, s)


def main():
    if len(sys.argv) < 4:
        print('Usage: {} <port> <baud> <command>'.format(sys.argv[0]))
        sys.exit(1)

    port = sys.argv[1]
    baud = int(sys.argv[2])
    cmd  = expand_carets(sys.argv[3])

    with S64Terminal(port, baud) as term:
        term.wait_for_prompt()
        print('Sending command: {!r}'.format(cmd))
        lines = term.send_command(cmd)
        print('--- {} line(s) received ---'.format(len(lines)))
        for line in lines:
            print(repr(line))


if __name__ == '__main__':
    main()

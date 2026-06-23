#!/usr/bin/env python3
"""Verify SWEET64 register set/read round-trips via the debug terminal.

Usage:
    python test_register_set.py <port> <baud>

Example:
    python test_register_set.py COM3 38400
"""

import sys
from s64terminal import S64Terminal, S64TerminalError
from s64registers import set_registers, read_registers


# (start_index, [values], description)
CASES = [
    (0, [0x2A],                     'single 64-bit reg, small value'),
    (1, [0x1234567890ABCDEF],       'single 64-bit reg, full 64-bit value'),
    (0, [0x11, 0x22, 0x33],         'multiple consecutive 64-bit regs'),
    (7, [0xA5],                     '8-bit reg (si64reg8flags), low byte only'),
]


def main():
    if len(sys.argv) < 3:
        print('Usage: {} <port> <baud>'.format(sys.argv[0]))
        sys.exit(1)

    port = sys.argv[1]
    baud = int(sys.argv[2])

    passed = 0
    failed = 0

    with S64Terminal(port, baud) as term:
        term.wait_for_prompt()

        for start, values, desc in CASES:
            set_registers(term, start, values)
            _, by_label = read_registers(term)
            by_index, _ = read_registers(term)  # also index-addressable

            ok = True
            detail = []
            for offset, expected in enumerate(values):
                idx = start + offset
                reg = by_index[idx] if idx < len(by_index) else None
                if reg is None:
                    ok = False
                    detail.append('reg 0x{:02X} missing'.format(idx))
                    continue
                # 8-bit registers only hold the low byte
                exp = expected & 0xFF if idx >= 7 else expected
                if reg.hex_value != exp:
                    ok = False
                    detail.append('reg 0x{:02X}: got 0x{:X} expected 0x{:X}'.format(
                        idx, reg.hex_value, exp))

            if ok:
                passed += 1
                print('PASS: {}'.format(desc))
            else:
                failed += 1
                print('FAIL: {} -- {}'.format(desc, '; '.join(detail)))

    print()
    print('{} passed, {} failed'.format(passed, failed))
    sys.exit(1 if failed else 0)


if __name__ == '__main__':
    main()

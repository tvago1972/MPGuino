"""MockTerminal — drop-in replacement for S64Terminal for offline testing.

Simulates the MPGuino debug monitor prompt/response behaviour using
canned output so the parsing and connectivity layers can be exercised
without hardware.

Usage:
    from mock_terminal import MockTerminal
    with MockTerminal() as term:
        by_mnemonic, by_opcode = fetch_instruction_set(term)
"""

from s64terminal import S64TerminalError

# Minimal canned ^I output covering a variety of instruction formats:
# invalid opcode, no operands, register only, register+operand, all operands.
# Format matches real monitor output: 'XX: (FFFF)OO [11] [22] [33]   mnemonic desc'
_CANNED_INSTRUCTION_LIST = [
    '^I',
    '00: (0000)00                  instrDone',
    '01: (2011)01 11               instrLdReg 0x11',
    '02: (2211)02 11 22            instrLdRegByteFromReg 0x11, (0x22)',
    '03: (A311)03 11 22 33         instrLdRegEEPROM 0x11, (0x22), (0x33)',
    '04: (0000)04                  INVALID INSTRUCTION',
    '05: (2031)05 11 22            instrAddReg 0x11, (0x22)',
    ']',
]

# Canned response for an unrecognised command — just the prompt.
_CANNED_PROMPT_ONLY = [']']


class MockTerminal:

    def __init__(self, instruction_lines=None):
        """instruction_lines: list of strings to return for the ^I command,
        including the leading '^I' echo and trailing ']' prompt.
        Defaults to a small built-in sample set."""
        self._instr_lines = instruction_lines or _CANNED_INSTRUCTION_LIST
        self._closed = False

    # ------------------------------------------------------------------
    # context manager
    # ------------------------------------------------------------------

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()

    def close(self):
        self._closed = True

    # ------------------------------------------------------------------
    # S64Terminal-compatible API
    # ------------------------------------------------------------------

    def wait_for_prompt(self):
        """Immediately succeeds — mock monitor is always ready."""
        if self._closed:
            raise S64TerminalError('MockTerminal is closed')

    def send_command(self, cmd):
        """Return canned output lines for known commands, prompt-only otherwise.
        Strips the trailing ']' prompt line before returning, matching
        S64Terminal.send_command() behaviour."""
        if self._closed:
            raise S64TerminalError('MockTerminal is closed')

        if cmd == '\x09':   # ^I instruction list
            lines = self._instr_lines
        else:
            lines = _CANNED_PROMPT_ONLY

        # strip leading/trailing prompt lines and return body only
        return [l for l in lines if l != ']']

    def sync(self):
        if self._closed:
            raise S64TerminalError('MockTerminal is closed')

import re
import serial


EOL = '\r'              # monitor executes commands on carriage return (0x0D);
                        # line feed (0x0A) is ignored by the monitor

READ_TIMEOUT = 2.0      # seconds with no data before a read is a stall
NUDGE_RETRIES = 2       # times to nudge with EOL before declaring a hang

# Prompts have no trailing newline, so they are matched against the pending
# (un-terminated) tail of the receive buffer.  fullmatch avoids false hits
# from partially-received data lines.
NORMAL_PROMPT_RE = re.compile(r'\]')
ASM_PROMPT_RE    = re.compile(r'[0-9A-Fa-f]{2}!:')
EITHER_PROMPT_RE = re.compile(r'\]|[0-9A-Fa-f]{2}!:')   # normal OR assembler


def is_asm_prompt(prompt_text):
    """True if a matched prompt string is the assembler 'XX!:' prompt."""
    return prompt_text is not None and ASM_PROMPT_RE.fullmatch(prompt_text) is not None


class S64TerminalError(Exception):
    pass


class S64Terminal:

    def __init__(self, port, baud=9600, timeout=READ_TIMEOUT):
        self._ser = serial.Serial(port, baud, timeout=timeout)
        self._buf = ''                  # unconsumed received characters
        self._ser.reset_input_buffer()

    def close(self):
        self._ser.close()

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()

    # ------------------------------------------------------------------
    # low-level receive
    # ------------------------------------------------------------------

    def _read_chunk(self):
        """Read whatever bytes are available (at least 1, blocking up to the
        serial timeout).  Returns decoded str, or '' on timeout."""
        n = self._ser.in_waiting
        chunk = self._ser.read(n if n > 0 else 1)
        return chunk.decode('ascii', errors='replace') if chunk else ''

    def _extract_lines(self):
        """Pull complete (newline-terminated) lines out of the buffer,
        leaving any unterminated tail behind.  Returns stripped lines."""
        parts = self._buf.split('\n')
        self._buf = parts.pop()         # trailing partial stays in the buffer
        return [p.strip('\r').strip() for p in parts]

    def _pending(self):
        """The current unterminated tail (a prompt arrives here)."""
        return self._buf.strip()

    def _matches_prompt(self, prompt_re):
        return prompt_re.fullmatch(self._pending()) is not None

    def _pump(self, prompt_re):
        """Read until the pending tail fully matches prompt_re, collecting
        completed output lines along the way.
        Returns (lines, prompt_text) on match, or (lines, None) on timeout."""
        lines = []
        while True:
            chunk = self._read_chunk()
            if chunk:
                self._buf += chunk
                lines.extend(self._extract_lines())
                if self._matches_prompt(prompt_re):
                    prompt = self._pending()
                    self._buf = ''
                    return lines, prompt
            else:
                # no data within the timeout
                if self._matches_prompt(prompt_re):
                    prompt = self._pending()
                    self._buf = ''
                    return lines, prompt
                return lines, None

    # ------------------------------------------------------------------
    # prompt synchronisation
    # ------------------------------------------------------------------

    def _pump_with_nudge(self, prompt_re, lines_out=None):
        """Pump to a prompt, nudging with EOL on stall.  Returns the matched
        prompt text, or None after exhausting nudges."""
        lines, prompt = self._pump(prompt_re)
        if lines_out is not None:
            lines_out.extend(lines)
        if prompt is not None:
            return prompt
        for _ in range(NUDGE_RETRIES):
            self._ser.write(EOL.encode('ascii'))
            lines, prompt = self._pump(prompt_re)
            if lines_out is not None:
                lines_out.extend(lines)
            if prompt is not None:
                return prompt
        return None

    # ------------------------------------------------------------------
    # public API
    # ------------------------------------------------------------------

    def wait_for_prompt(self):
        """Block until the monitor emits the normal ']' prompt."""
        if self._pump_with_nudge(NORMAL_PROMPT_RE) is None:
            raise S64TerminalError(
                'monitor did not reach prompt - possible hang or wrong baud rate')

    def exchange(self, text, prompt_re=NORMAL_PROMPT_RE, flush=True):
        """Send text (terminated with CR) and collect output lines up to the
        next prompt.  prompt_re selects which prompt(s) terminate the read.
        Returns (lines, prompt_text) where prompt_text is the matched prompt
        (useful when prompt_re can match more than one prompt).
        flush=False skips the input-buffer reset for interactive modes where
        output may already be in flight."""
        if flush:
            self._ser.reset_input_buffer()
            self._buf = ''
        self._ser.write((text + EOL).encode('ascii'))
        lines = []
        prompt = self._pump_with_nudge(prompt_re, lines)
        if prompt is None:
            raise S64TerminalError(
                'monitor stopped responding after input: {!r}'.format(text))
        return [l for l in lines if l], prompt

    def send_command(self, cmd, prompt_re=NORMAL_PROMPT_RE):
        """Send a command and return the list of non-empty output lines up to
        the next prompt.  Convenience wrapper over exchange()."""
        lines, _ = self.exchange(cmd, prompt_re, flush=True)
        return lines

    def sync(self):
        """Resynchronise with the monitor by nudging to the normal prompt."""
        self._ser.reset_input_buffer()
        self._buf = ''
        if not self._pump_with_nudge(NORMAL_PROMPT_RE):
            raise S64TerminalError('could not resync with monitor')

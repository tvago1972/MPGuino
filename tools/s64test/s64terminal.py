import serial


PROMPT = ']'
EOL = '\r'              # monitor executes commands on carriage return (0x0D);
                       # line feed (0x0A) is ignored by the monitor
PROBE_TIMEOUT = 0.5     # seconds to wait after sending probe CR
DEFAULT_TIMEOUT = 5.0   # seconds before a readline is considered stalled
DRAIN_TIMEOUT = 10.0    # seconds to wait per line when draining mid-listing


class S64TerminalError(Exception):
    pass


class S64Terminal:

    def __init__(self, port, baud=9600, timeout=DEFAULT_TIMEOUT):
        self._timeout = timeout
        self._ser = serial.Serial(port, baud, timeout=timeout)
        # discard anything already in the buffer
        self._ser.reset_input_buffer()

    def close(self):
        self._ser.close()

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()

    # ------------------------------------------------------------------
    # low-level line reader
    # ------------------------------------------------------------------

    def _readline(self):
        """Read one line and return it stripped of CR/LF.
        Returns None on timeout (pyserial returns b'' when no newline arrives
        before the timeout), or a str (possibly empty) for a received line."""
        raw = self._ser.readline()
        if not raw:
            return None     # timeout — no data received
        return raw.decode('ascii', errors='replace').strip()

    # ------------------------------------------------------------------
    # prompt detection
    # ------------------------------------------------------------------

    def _is_prompt(self, line):
        return line == PROMPT

    def _drain_to_prompt(self, lines=None):
        """Drain output lines until a bare ']' prompt is seen or reads time
        out twice in a row.  Any non-prompt lines are appended to `lines`
        if provided.  Sends a bare newline only after the first timeout to
        nudge the monitor if it is already idle.
        Returns True if the prompt was found, False on double timeout."""
        prev_timeout = self._ser.timeout
        probed = False
        try:
            self._ser.timeout = DRAIN_TIMEOUT
            while True:
                line = self._readline()
                if line is None:
                    if probed:
                        return False    # second timeout - genuine hang
                    # first timeout - nudge the monitor and try once more
                    self._ser.write(EOL.encode('ascii'))
                    self._ser.timeout = PROBE_TIMEOUT
                    probed = True
                    continue
                if self._is_prompt(line):
                    return True
                if line and lines is not None:
                    lines.append(line)
                # reset probe state on any received line
                if probed:
                    self._ser.timeout = DRAIN_TIMEOUT
                    probed = False
        finally:
            self._ser.timeout = prev_timeout

    # ------------------------------------------------------------------
    # public API
    # ------------------------------------------------------------------

    def wait_for_prompt(self):
        """Block until the monitor emits a bare ']' prompt line.
        Drains on readline timeout to distinguish stalled output from a hang.
        Raises S64TerminalError if the monitor appears genuinely hung."""
        while True:
            line = self._readline()

            if line is None:
                # readline timed out - drain to see if monitor is waiting
                if self._drain_to_prompt():
                    return
                raise S64TerminalError(
                    'monitor did not respond to prompt probe - '
                    'possible hang or wrong baud rate'
                )

            if self._is_prompt(line):
                return

    def send_command(self, cmd):
        """Send a command string and collect all output lines until the
        next prompt.  Returns a list of non-empty output lines.
        The input buffer is flushed before sending to clear any stale
        prompt characters left over from the previous command."""
        self._ser.reset_input_buffer()
        self._ser.write((cmd + EOL).encode('ascii'))
        lines = []
        while True:
            line = self._readline()

            if self._is_prompt(line):
                return lines

            if line is None:
                # readline timed out - drain remaining output into lines
                if self._drain_to_prompt(lines):
                    return lines
                raise S64TerminalError(
                    'monitor stopped responding mid-output '
                    'after command: {!r}'.format(cmd)
                )

            if line:
                lines.append(line)

    def sync(self):
        """Resynchronise with the monitor after an unexpected state by
        sending a newline and waiting for the prompt.  Safe to call at
        any time."""
        self._ser.reset_input_buffer()
        if not self._drain_to_prompt():
            raise S64TerminalError('could not resync with monitor')

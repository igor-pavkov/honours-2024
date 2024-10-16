static void mp_throttle(struct tty_struct *tty)
{
	struct sb_uart_state *state = tty->driver_data;

	if (I_IXOFF(tty))
		mp_send_xchar(tty, STOP_CHAR(tty));

	if (tty->termios.c_cflag & CRTSCTS)
		uart_clear_mctrl(state->port, TIOCM_RTS);
}
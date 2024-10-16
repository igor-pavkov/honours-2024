static void mp_update_termios(struct sb_uart_state *state)
{
	struct tty_struct *tty = state->info->tty;
	struct sb_uart_port *port = state->port;

	if (!(tty->flags & (1 << TTY_IO_ERROR))) {
		mp_change_speed(state, NULL);

		if (tty->termios.c_cflag & CBAUD)
			uart_set_mctrl(port, TIOCM_DTR | TIOCM_RTS);
	}
}
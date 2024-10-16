static void mp_unthrottle(struct tty_struct *tty)
{
	struct sb_uart_state *state = tty->driver_data;
	struct sb_uart_port *port = state->port;

	if (I_IXOFF(tty)) {
		if (port->x_char)
			port->x_char = 0;
		else
			mp_send_xchar(tty, START_CHAR(tty));
	}

	if (tty->termios.c_cflag & CRTSCTS)
		uart_set_mctrl(port, TIOCM_RTS);
}
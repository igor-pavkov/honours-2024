static void mp_change_speed(struct sb_uart_state *state, struct MP_TERMIOS *old_termios)
{
	struct tty_struct *tty = state->info->tty;
	struct sb_uart_port *port = state->port;

	if (!tty || port->type == PORT_UNKNOWN)
		return;

	if (tty->termios.c_cflag & CRTSCTS)
		state->info->flags |= UIF_CTS_FLOW;
	else
		state->info->flags &= ~UIF_CTS_FLOW;

	if (tty->termios.c_cflag & CLOCAL)
		state->info->flags &= ~UIF_CHECK_CD;
	else
		state->info->flags |= UIF_CHECK_CD;

	port->ops->set_termios(port, &tty->termios, old_termios);
}
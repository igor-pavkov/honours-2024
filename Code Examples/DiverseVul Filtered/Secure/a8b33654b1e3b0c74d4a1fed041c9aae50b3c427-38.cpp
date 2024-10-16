static int mp_break_ctl(struct tty_struct *tty, int break_state)
{
	struct sb_uart_state *state = tty->driver_data;
	struct sb_uart_port *port = state->port;

	MP_STATE_LOCK(state);

	if (port->type != PORT_UNKNOWN)
		port->ops->break_ctl(port, break_state);

	MP_STATE_UNLOCK(state);
	return 0;
}
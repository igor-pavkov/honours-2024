static void mp_unconfigure_port(struct uart_driver *drv, struct sb_uart_state *state)
{
	struct sb_uart_port *port = state->port;
	struct sb_uart_info *info = state->info;

	if (info && info->tty)
		tty_hangup(info->tty);

	MP_STATE_LOCK(state);

	state->info = NULL;

	if (port->type != PORT_UNKNOWN)
		port->ops->release_port(port);

	port->type = PORT_UNKNOWN;

	if (info) {
		tasklet_kill(&info->tlet);
		kfree(info);
	}

	MP_STATE_UNLOCK(state);
}
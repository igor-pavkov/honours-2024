static int mp_do_autoconfig(struct sb_uart_state *state)
{
	struct sb_uart_port *port = state->port;
	int flags, ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (mutex_lock_interruptible(&state->mutex))
		return -ERESTARTSYS;
	ret = -EBUSY;
	if (uart_users(state) == 1) {
		mp_shutdown(state);

		if (port->type != PORT_UNKNOWN)
			port->ops->release_port(port);

		flags = UART_CONFIG_TYPE;
		if (port->flags & UPF_AUTO_IRQ)
			flags |= UART_CONFIG_IRQ;

		port->ops->config_port(port, flags);

		ret = mp_startup(state, 1);
	}
	MP_STATE_UNLOCK(state);
	return ret;
}
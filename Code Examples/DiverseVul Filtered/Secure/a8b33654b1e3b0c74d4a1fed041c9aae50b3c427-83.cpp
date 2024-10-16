static int mp_add_one_port(struct uart_driver *drv, struct sb_uart_port *port)
{
	struct sb_uart_state *state;
	int ret = 0;


	if (port->line >= drv->nr)
		return -EINVAL;

	state = drv->state + port->line;

	MP_MUTEX_LOCK(mp_mutex);
	if (state->port) {
		ret = -EINVAL;
		goto out;
	}

	state->port = port;

	spin_lock_init(&port->lock);
	port->cons = drv->cons;
	port->info = state->info;

	mp_configure_port(drv, state, port);

	tty_register_device(drv->tty_driver, port->line, port->dev);

out:
	MP_MUTEX_UNLOCK(mp_mutex);


	return ret;
}
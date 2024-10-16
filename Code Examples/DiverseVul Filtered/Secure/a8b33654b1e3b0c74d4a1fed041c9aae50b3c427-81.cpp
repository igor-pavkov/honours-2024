static int mp_remove_one_port(struct uart_driver *drv, struct sb_uart_port *port)
{
	struct sb_uart_state *state = drv->state + port->line;

	if (state->port != port)
		printk(KERN_ALERT "Removing wrong port: %p != %p\n",
				state->port, port);

	MP_MUTEX_LOCK(mp_mutex);

	tty_unregister_device(drv->tty_driver, port->line);

	mp_unconfigure_port(drv, state);
	state->port = NULL;
	MP_MUTEX_UNLOCK(mp_mutex);

	return 0;
}
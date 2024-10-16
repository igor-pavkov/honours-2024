static void __mp_start(struct tty_struct *tty)
{
	struct sb_uart_state *state = tty->driver_data;
	struct sb_uart_port *port = state->port;

	if (!uart_circ_empty(&state->info->xmit) && state->info->xmit.buf &&
			!tty->stopped && !tty->hw_stopped)
		port->ops->start_tx(port);
}
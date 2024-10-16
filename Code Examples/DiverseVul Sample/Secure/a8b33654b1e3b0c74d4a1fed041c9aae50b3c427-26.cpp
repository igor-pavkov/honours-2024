static void mp_stop(struct tty_struct *tty)
{
	struct sb_uart_state *state = tty->driver_data;
	struct sb_uart_port *port = state->port;
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);
	port->ops->stop_tx(port);
	spin_unlock_irqrestore(&port->lock, flags);
}
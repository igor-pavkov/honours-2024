static void mp_send_xchar(struct tty_struct *tty, char ch)
{
	struct sb_uart_state *state = tty->driver_data;
	struct sb_uart_port *port = state->port;
	unsigned long flags;

	if (port->ops->send_xchar)
		port->ops->send_xchar(port, ch);
	else {
		port->x_char = ch;
		if (ch) {
			spin_lock_irqsave(&port->lock, flags);
			port->ops->start_tx(port);
			spin_unlock_irqrestore(&port->lock, flags);
		}
	}
}
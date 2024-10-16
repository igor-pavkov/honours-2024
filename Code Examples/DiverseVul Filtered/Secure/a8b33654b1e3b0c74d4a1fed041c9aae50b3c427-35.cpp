static void mp_flush_buffer(struct tty_struct *tty)
{
	struct sb_uart_state *state = tty->driver_data;
	struct sb_uart_port *port;
	unsigned long flags;

	if (!state || !state->info) {
		return;
	}

	port = state->port;
	spin_lock_irqsave(&port->lock, flags);
	uart_circ_clear(&state->info->xmit);
	spin_unlock_irqrestore(&port->lock, flags);
	wake_up_interruptible(&tty->write_wait);
	tty_wakeup(tty);
}
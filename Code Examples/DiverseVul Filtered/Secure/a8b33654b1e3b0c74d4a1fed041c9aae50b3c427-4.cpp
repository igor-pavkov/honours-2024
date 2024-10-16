static int mp_tiocmget(struct tty_struct *tty)
{
	struct sb_uart_state *state = tty->driver_data;
	struct sb_uart_port *port = state->port;
	int result = -EIO;

	MP_STATE_LOCK(state);
	if (!(tty->flags & (1 << TTY_IO_ERROR))) {
		result = port->mctrl;
		spin_lock_irq(&port->lock);
		result |= port->ops->get_mctrl(port);
		spin_unlock_irq(&port->lock);
	}
	MP_STATE_UNLOCK(state);
	return result;
}
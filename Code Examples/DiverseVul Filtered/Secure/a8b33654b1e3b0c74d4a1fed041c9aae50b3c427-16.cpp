static int mp_tiocmset(struct tty_struct *tty, unsigned int set, unsigned int clear)
{
	struct sb_uart_state *state = tty->driver_data;
	struct sb_uart_port *port = state->port;
	int ret = -EIO;


	MP_STATE_LOCK(state);
	if (!(tty->flags & (1 << TTY_IO_ERROR))) {
		mp_update_mctrl(port, set, clear);
		ret = 0;
	}
	MP_STATE_UNLOCK(state);

	return ret;
}
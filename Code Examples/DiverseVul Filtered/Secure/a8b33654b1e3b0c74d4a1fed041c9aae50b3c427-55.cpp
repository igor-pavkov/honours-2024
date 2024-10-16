static int mp_startup(struct sb_uart_state *state, int init_hw)
{
	struct sb_uart_info *info = state->info;
	struct sb_uart_port *port = state->port;
	unsigned long page;
	int retval = 0;

	if (info->flags & UIF_INITIALIZED)
		return 0;

	if (info->tty)
		set_bit(TTY_IO_ERROR, &info->tty->flags);

	if (port->type == PORT_UNKNOWN)
		return 0;

	if (!info->xmit.buf) {
		page = get_zeroed_page(GFP_KERNEL);
		if (!page)
			return -ENOMEM;

		info->xmit.buf = (unsigned char *) page;
			
		uart_circ_clear(&info->xmit);
	}

	retval = port->ops->startup(port);
	if (retval == 0) {
		if (init_hw) {
			mp_change_speed(state, NULL);

			if (info->tty->termios.c_cflag & CBAUD)
				uart_set_mctrl(port, TIOCM_RTS | TIOCM_DTR);
		}

		info->flags |= UIF_INITIALIZED;


		clear_bit(TTY_IO_ERROR, &info->tty->flags);
	}

	if (retval && capable(CAP_SYS_ADMIN))
		retval = 0;

	return retval;
}
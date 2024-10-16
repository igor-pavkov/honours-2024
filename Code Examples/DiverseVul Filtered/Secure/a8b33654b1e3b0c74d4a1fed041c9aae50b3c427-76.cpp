static int mp_block_til_ready(struct file *filp, struct sb_uart_state *state)
{
	DECLARE_WAITQUEUE(wait, current);
	struct sb_uart_info *info = state->info;
	struct sb_uart_port *port = state->port;
	unsigned int mctrl;

	info->blocked_open++;
	state->count--;

	add_wait_queue(&info->open_wait, &wait);
	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);

		if (tty_hung_up_p(filp) || info->tty == NULL)
			break;

		if (!(info->flags & UIF_INITIALIZED))
			break;

		if ((filp->f_flags & O_NONBLOCK) ||
				(info->tty->termios.c_cflag & CLOCAL) ||
				(info->tty->flags & (1 << TTY_IO_ERROR))) {
			break;
		}

		if (info->tty->termios.c_cflag & CBAUD)
			uart_set_mctrl(port, TIOCM_DTR);

		spin_lock_irq(&port->lock);
		port->ops->enable_ms(port);
		mctrl = port->ops->get_mctrl(port);
		spin_unlock_irq(&port->lock);
		if (mctrl & TIOCM_CAR)
			break;

		MP_STATE_UNLOCK(state);
		schedule();
		MP_STATE_LOCK(state);

		if (signal_pending(current))
			break;
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&info->open_wait, &wait);

	state->count++;
	info->blocked_open--;

	if (signal_pending(current))
		return -ERESTARTSYS;

	if (!info->tty || tty_hung_up_p(filp))
		return -EAGAIN;

	return 0;
}
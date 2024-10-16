static void n_tty_receive_break(struct tty_struct *tty)
{
	struct n_tty_data *ldata = tty->disc_data;

	if (I_IGNBRK(tty))
		return;
	if (I_BRKINT(tty)) {
		isig(SIGINT, tty);
		if (!L_NOFLSH(tty)) {
			/* flushing needs exclusive termios_rwsem */
			up_read(&tty->termios_rwsem);
			n_tty_flush_buffer(tty);
			tty_driver_flush_buffer(tty);
			down_read(&tty->termios_rwsem);
		}
		return;
	}
	if (I_PARMRK(tty)) {
		put_tty_queue('\377', ldata);
		put_tty_queue('\0', ldata);
	}
	put_tty_queue('\0', ldata);
	if (waitqueue_active(&tty->read_wait))
		wake_up_interruptible(&tty->read_wait);
}
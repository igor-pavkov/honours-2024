static void n_tty_receive_parity_error(struct tty_struct *tty, unsigned char c)
{
	struct n_tty_data *ldata = tty->disc_data;

	if (I_IGNPAR(tty))
		return;
	if (I_PARMRK(tty)) {
		put_tty_queue('\377', ldata);
		put_tty_queue('\0', ldata);
		put_tty_queue(c, ldata);
	} else	if (I_INPCK(tty))
		put_tty_queue('\0', ldata);
	else
		put_tty_queue(c, ldata);
	if (waitqueue_active(&tty->read_wait))
		wake_up_interruptible(&tty->read_wait);
}
static void n_tty_fasync(struct tty_struct *tty, int on)
{
	struct n_tty_data *ldata = tty->disc_data;

	if (!waitqueue_active(&tty->read_wait)) {
		if (on)
			ldata->minimum_to_wake = 1;
		else if (!tty->fasync)
			ldata->minimum_to_wake = N_TTY_BUF_SIZE;
	}
}
static int n_tty_open(struct tty_struct *tty)
{
	struct n_tty_data *ldata;

	/* Currently a malloc failure here can panic */
	ldata = vmalloc(sizeof(*ldata));
	if (!ldata)
		goto err;

	ldata->overrun_time = jiffies;
	mutex_init(&ldata->atomic_read_lock);
	mutex_init(&ldata->output_lock);

	tty->disc_data = ldata;
	reset_buffer_flags(tty->disc_data);
	ldata->column = 0;
	ldata->canon_column = 0;
	ldata->minimum_to_wake = 1;
	ldata->num_overrun = 0;
	ldata->no_room = 0;
	ldata->lnext = 0;
	tty->closing = 0;
	/* indicate buffer work may resume */
	clear_bit(TTY_LDISC_HALTED, &tty->flags);
	n_tty_set_termios(tty, NULL);
	tty_unthrottle(tty);

	return 0;
err:
	return -ENOMEM;
}
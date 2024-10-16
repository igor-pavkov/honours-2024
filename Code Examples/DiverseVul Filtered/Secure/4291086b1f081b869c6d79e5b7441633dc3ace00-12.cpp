static int receive_room(struct tty_struct *tty)
{
	struct n_tty_data *ldata = tty->disc_data;
	int left;

	if (I_PARMRK(tty)) {
		/* Multiply read_cnt by 3, since each byte might take up to
		 * three times as many spaces when PARMRK is set (depending on
		 * its flags, e.g. parity error). */
		left = N_TTY_BUF_SIZE - read_cnt(ldata) * 3 - 1;
	} else
		left = N_TTY_BUF_SIZE - read_cnt(ldata) - 1;

	/*
	 * If we are doing input canonicalization, and there are no
	 * pending newlines, let characters through without limit, so
	 * that erase characters will be handled.  Other excess
	 * characters will be beeped.
	 */
	if (left <= 0)
		left = ldata->icanon && ldata->canon_head == ldata->read_tail;

	return left;
}
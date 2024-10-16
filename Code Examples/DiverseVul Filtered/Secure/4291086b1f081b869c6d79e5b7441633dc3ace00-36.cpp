static inline int input_available_p(struct tty_struct *tty, int poll)
{
	struct n_tty_data *ldata = tty->disc_data;
	int amt = poll && !TIME_CHAR(tty) && MIN_CHAR(tty) ? MIN_CHAR(tty) : 1;

	if (ldata->icanon && !L_EXTPROC(tty))
		return ldata->canon_head != ldata->read_tail;
	else
		return read_cnt(ldata) >= amt;
}
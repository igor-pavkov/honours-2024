static ssize_t chars_in_buffer(struct tty_struct *tty)
{
	struct n_tty_data *ldata = tty->disc_data;
	ssize_t n = 0;

	if (!ldata->icanon)
		n = read_cnt(ldata);
	else
		n = ldata->canon_head - ldata->read_tail;
	return n;
}
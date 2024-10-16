static unsigned long inq_canon(struct n_tty_data *ldata)
{
	size_t nr, head, tail;

	if (ldata->canon_head == ldata->read_tail)
		return 0;
	head = ldata->canon_head;
	tail = ldata->read_tail;
	nr = head - tail;
	/* Skip EOF-chars.. */
	while (head != tail) {
		if (test_bit(tail & (N_TTY_BUF_SIZE - 1), ldata->read_flags) &&
		    read_buf(ldata, tail) == __DISABLED_CHAR)
			nr--;
		tail++;
	}
	return nr;
}
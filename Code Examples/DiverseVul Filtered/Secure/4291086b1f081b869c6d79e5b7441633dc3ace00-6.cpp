static int copy_from_read_buf(struct tty_struct *tty,
				      unsigned char __user **b,
				      size_t *nr)

{
	struct n_tty_data *ldata = tty->disc_data;
	int retval;
	size_t n;
	bool is_eof;
	size_t tail = ldata->read_tail & (N_TTY_BUF_SIZE - 1);

	retval = 0;
	n = min(read_cnt(ldata), N_TTY_BUF_SIZE - tail);
	n = min(*nr, n);
	if (n) {
		retval = copy_to_user(*b, read_buf_addr(ldata, tail), n);
		n -= retval;
		is_eof = n == 1 && read_buf(ldata, tail) == EOF_CHAR(tty);
		tty_audit_add_data(tty, read_buf_addr(ldata, tail), n,
				ldata->icanon);
		ldata->read_tail += n;
		/* Turn single EOF into zero-length read */
		if (L_EXTPROC(tty) && ldata->icanon && is_eof && !read_cnt(ldata))
			n = 0;
		*b += n;
		*nr -= n;
	}
	return retval;
}
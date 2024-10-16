static int canon_copy_from_read_buf(struct tty_struct *tty,
				    unsigned char __user **b,
				    size_t *nr)
{
	struct n_tty_data *ldata = tty->disc_data;
	size_t n, size, more, c;
	size_t eol;
	size_t tail;
	int ret, found = 0;
	bool eof_push = 0;

	/* N.B. avoid overrun if nr == 0 */
	n = min(*nr, read_cnt(ldata));
	if (!n)
		return 0;

	tail = ldata->read_tail & (N_TTY_BUF_SIZE - 1);
	size = min_t(size_t, tail + n, N_TTY_BUF_SIZE);

	n_tty_trace("%s: nr:%zu tail:%zu n:%zu size:%zu\n",
		    __func__, *nr, tail, n, size);

	eol = find_next_bit(ldata->read_flags, size, tail);
	more = n - (size - tail);
	if (eol == N_TTY_BUF_SIZE && more) {
		/* scan wrapped without finding set bit */
		eol = find_next_bit(ldata->read_flags, more, 0);
		if (eol != more)
			found = 1;
	} else if (eol != size)
		found = 1;

	size = N_TTY_BUF_SIZE - tail;
	n = eol - tail;
	if (n > 4096)
		n += 4096;
	n += found;
	c = n;

	if (found && !ldata->push && read_buf(ldata, eol) == __DISABLED_CHAR) {
		n--;
		eof_push = !n && ldata->read_tail != ldata->line_start;
	}

	n_tty_trace("%s: eol:%zu found:%d n:%zu c:%zu size:%zu more:%zu\n",
		    __func__, eol, found, n, c, size, more);

	if (n > size) {
		ret = copy_to_user(*b, read_buf_addr(ldata, tail), size);
		if (ret)
			return -EFAULT;
		ret = copy_to_user(*b + size, ldata->read_buf, n - size);
	} else
		ret = copy_to_user(*b, read_buf_addr(ldata, tail), n);

	if (ret)
		return -EFAULT;
	*b += n;
	*nr -= n;

	if (found)
		clear_bit(eol, ldata->read_flags);
	smp_mb__after_clear_bit();
	ldata->read_tail += c;

	if (found) {
		if (!ldata->push)
			ldata->line_start = ldata->read_tail;
		else
			ldata->push = 0;
		tty_audit_push(tty);
	}
	return eof_push ? -EAGAIN : 0;
}
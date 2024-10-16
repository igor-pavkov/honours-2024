static int gen_wants_ndx(int desired_ndx, int flist_num)
{
	static int next_ndx = -1;
	static int done_cnt = 0;
	static BOOL got_eof = False;

	if (got_eof)
		return 0;

	/* TODO: integrate gen-reading I/O into perform_io() so this is not needed? */
	io_flush(FULL_FLUSH);

	while (next_ndx < desired_ndx) {
		if (inc_recurse && flist_num <= done_cnt)
			return 0;
		if (next_ndx >= 0)
			no_batched_update(next_ndx, False);
		if ((next_ndx = read_int(batch_gen_fd)) < 0) {
			if (inc_recurse) {
				done_cnt++;
				continue;
			}
			got_eof = True;
			return 0;
		}
	}

	if (next_ndx == desired_ndx) {
		next_ndx = -1;
		return 1;
	}

	return 0;
}
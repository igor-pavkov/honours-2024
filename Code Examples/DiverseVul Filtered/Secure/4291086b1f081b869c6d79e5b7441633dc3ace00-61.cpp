static void reset_buffer_flags(struct n_tty_data *ldata)
{
	ldata->read_head = ldata->canon_head = ldata->read_tail = 0;
	ldata->echo_head = ldata->echo_tail = ldata->echo_commit = 0;
	ldata->echo_mark = 0;
	ldata->line_start = 0;

	ldata->erasing = 0;
	bitmap_zero(ldata->read_flags, N_TTY_BUF_SIZE);
	ldata->push = 0;
}
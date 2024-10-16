static void progress_meter(int flag)
{
	int notty;

	if (option_mask32 & WGET_OPT_QUIET)
		return;

	/* Don't save progress to log file */
	if (G.log_fd >= 0)
		return;

	if (flag == PROGRESS_START)
		bb_progress_init(&G.pmt, G.curfile);

	notty = bb_progress_update(&G.pmt,
			G.beg_range,
			G.transferred,
			(G.chunked || !G.got_clen) ? 0 : G.beg_range + G.transferred + G.content_len
	);

	if (flag == PROGRESS_END) {
		bb_progress_free(&G.pmt);
		if (notty == 0)
			bb_putchar_stderr('\n'); /* it's tty */
		G.transferred = 0;
	}
}
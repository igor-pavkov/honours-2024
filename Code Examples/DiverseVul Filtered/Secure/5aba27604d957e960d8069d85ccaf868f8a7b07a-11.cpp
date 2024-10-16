static void muxer_report_progress(MovieWriter *mw)
{
	if (mw->movie->progress_cbk) {
		mw->movie->progress_cbk(mw->movie->progress_cbk_udta, mw->nb_done, mw->total_samples);
	} else {
		gf_set_progress("ISO File Writing", mw->nb_done, mw->total_samples);
	}
}
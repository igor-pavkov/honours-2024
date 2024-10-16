static void ocfs2_free_write_ctxt(struct ocfs2_write_ctxt *wc)
{
	ocfs2_unlock_and_free_pages(wc->w_pages, wc->w_num_pages);

	brelse(wc->w_di_bh);
	kfree(wc);
}
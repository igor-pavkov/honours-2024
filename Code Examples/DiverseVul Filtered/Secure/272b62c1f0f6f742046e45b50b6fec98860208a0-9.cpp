static void ocfs2_set_target_boundaries(struct ocfs2_super *osb,
					struct ocfs2_write_ctxt *wc,
					loff_t pos, unsigned len, int alloc)
{
	struct ocfs2_write_cluster_desc *desc;

	wc->w_target_from = pos & (PAGE_CACHE_SIZE - 1);
	wc->w_target_to = wc->w_target_from + len;

	if (alloc == 0)
		return;

	/*
	 * Allocating write - we may have different boundaries based
	 * on page size and cluster size.
	 *
	 * NOTE: We can no longer compute one value from the other as
	 * the actual write length and user provided length may be
	 * different.
	 */

	if (wc->w_large_pages) {
		/*
		 * We only care about the 1st and last cluster within
		 * our range and whether they should be zero'd or not. Either
		 * value may be extended out to the start/end of a
		 * newly allocated cluster.
		 */
		desc = &wc->w_desc[0];
		if (desc->c_needs_zero)
			ocfs2_figure_cluster_boundaries(osb,
							desc->c_cpos,
							&wc->w_target_from,
							NULL);

		desc = &wc->w_desc[wc->w_clen - 1];
		if (desc->c_needs_zero)
			ocfs2_figure_cluster_boundaries(osb,
							desc->c_cpos,
							NULL,
							&wc->w_target_to);
	} else {
		wc->w_target_from = 0;
		wc->w_target_to = PAGE_CACHE_SIZE;
	}
}
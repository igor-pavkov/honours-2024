static void jbd2_freeze_jh_data(struct journal_head *jh)
{
	struct page *page;
	int offset;
	char *source;
	struct buffer_head *bh = jh2bh(jh);

	J_EXPECT_JH(jh, buffer_uptodate(bh), "Possible IO failure.\n");
	page = bh->b_page;
	offset = offset_in_page(bh->b_data);
	source = kmap_atomic(page);
	/* Fire data frozen trigger just before we copy the data */
	jbd2_buffer_frozen_trigger(jh, source + offset, jh->b_triggers);
	memcpy(jh->b_frozen_data, source + offset, bh->b_size);
	kunmap_atomic(source);

	/*
	 * Now that the frozen data is saved off, we need to store any matching
	 * triggers.
	 */
	jh->b_frozen_triggers = jh->b_triggers;
}
static int ocfs2_releasepage(struct page *page, gfp_t wait)
{
	journal_t *journal = OCFS2_SB(page->mapping->host->i_sb)->journal->j_journal;

	if (!page_has_buffers(page))
		return 0;
	return jbd2_journal_try_to_free_buffers(journal, page, wait);
}
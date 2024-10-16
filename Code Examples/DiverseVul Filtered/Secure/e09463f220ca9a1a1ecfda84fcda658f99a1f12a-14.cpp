static void __jbd2_journal_unfile_buffer(struct journal_head *jh)
{
	__jbd2_journal_temp_unlink_buffer(jh);
	jh->b_transaction = NULL;
	jbd2_journal_put_journal_head(jh);
}
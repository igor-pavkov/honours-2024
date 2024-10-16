static void sub_reserved_credits(journal_t *journal, int blocks)
{
	atomic_sub(blocks, &journal->j_reserved_credits);
	wake_up(&journal->j_wait_reserved);
}
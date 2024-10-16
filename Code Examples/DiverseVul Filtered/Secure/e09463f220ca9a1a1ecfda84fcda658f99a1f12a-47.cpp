void jbd2_journal_destroy_transaction_cache(void)
{
	kmem_cache_destroy(transaction_cache);
	transaction_cache = NULL;
}
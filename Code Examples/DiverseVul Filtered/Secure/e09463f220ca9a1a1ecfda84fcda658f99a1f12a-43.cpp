void jbd2_journal_free_transaction(transaction_t *transaction)
{
	if (unlikely(ZERO_OR_NULL_PTR(transaction)))
		return;
	kmem_cache_free(transaction_cache, transaction);
}
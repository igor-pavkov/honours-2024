void jbd2_journal_free_reserved(handle_t *handle)
{
	journal_t *journal = handle->h_journal;

	WARN_ON(!handle->h_reserved);
	sub_reserved_credits(journal, handle->h_buffer_credits);
	jbd2_free_handle(handle);
}
int jbd2__journal_restart(handle_t *handle, int nblocks, gfp_t gfp_mask)
{
	transaction_t *transaction = handle->h_transaction;
	journal_t *journal;
	tid_t		tid;
	int		need_to_start, ret;

	/* If we've had an abort of any type, don't even think about
	 * actually doing the restart! */
	if (is_handle_aborted(handle))
		return 0;
	journal = transaction->t_journal;

	/*
	 * First unlink the handle from its current transaction, and start the
	 * commit on that.
	 */
	J_ASSERT(atomic_read(&transaction->t_updates) > 0);
	J_ASSERT(journal_current_handle() == handle);

	read_lock(&journal->j_state_lock);
	spin_lock(&transaction->t_handle_lock);
	atomic_sub(handle->h_buffer_credits,
		   &transaction->t_outstanding_credits);
	if (handle->h_rsv_handle) {
		sub_reserved_credits(journal,
				     handle->h_rsv_handle->h_buffer_credits);
	}
	if (atomic_dec_and_test(&transaction->t_updates))
		wake_up(&journal->j_wait_updates);
	tid = transaction->t_tid;
	spin_unlock(&transaction->t_handle_lock);
	handle->h_transaction = NULL;
	current->journal_info = NULL;

	jbd_debug(2, "restarting handle %p\n", handle);
	need_to_start = !tid_geq(journal->j_commit_request, tid);
	read_unlock(&journal->j_state_lock);
	if (need_to_start)
		jbd2_log_start_commit(journal, tid);

	rwsem_release(&journal->j_trans_commit_map, 1, _THIS_IP_);
	handle->h_buffer_credits = nblocks;
	/*
	 * Restore the original nofs context because the journal restart
	 * is basically the same thing as journal stop and start.
	 * start_this_handle will start a new nofs context.
	 */
	memalloc_nofs_restore(handle->saved_alloc_context);
	ret = start_this_handle(journal, handle, gfp_mask);
	return ret;
}
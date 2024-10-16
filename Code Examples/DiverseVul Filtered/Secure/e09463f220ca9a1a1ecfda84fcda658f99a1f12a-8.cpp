int jbd2_journal_stop(handle_t *handle)
{
	transaction_t *transaction = handle->h_transaction;
	journal_t *journal;
	int err = 0, wait_for_commit = 0;
	tid_t tid;
	pid_t pid;

	if (!transaction) {
		/*
		 * Handle is already detached from the transaction so
		 * there is nothing to do other than decrease a refcount,
		 * or free the handle if refcount drops to zero
		 */
		if (--handle->h_ref > 0) {
			jbd_debug(4, "h_ref %d -> %d\n", handle->h_ref + 1,
							 handle->h_ref);
			return err;
		} else {
			if (handle->h_rsv_handle)
				jbd2_free_handle(handle->h_rsv_handle);
			goto free_and_exit;
		}
	}
	journal = transaction->t_journal;

	J_ASSERT(journal_current_handle() == handle);

	if (is_handle_aborted(handle))
		err = -EIO;
	else
		J_ASSERT(atomic_read(&transaction->t_updates) > 0);

	if (--handle->h_ref > 0) {
		jbd_debug(4, "h_ref %d -> %d\n", handle->h_ref + 1,
			  handle->h_ref);
		return err;
	}

	jbd_debug(4, "Handle %p going down\n", handle);
	trace_jbd2_handle_stats(journal->j_fs_dev->bd_dev,
				transaction->t_tid,
				handle->h_type, handle->h_line_no,
				jiffies - handle->h_start_jiffies,
				handle->h_sync, handle->h_requested_credits,
				(handle->h_requested_credits -
				 handle->h_buffer_credits));

	/*
	 * Implement synchronous transaction batching.  If the handle
	 * was synchronous, don't force a commit immediately.  Let's
	 * yield and let another thread piggyback onto this
	 * transaction.  Keep doing that while new threads continue to
	 * arrive.  It doesn't cost much - we're about to run a commit
	 * and sleep on IO anyway.  Speeds up many-threaded, many-dir
	 * operations by 30x or more...
	 *
	 * We try and optimize the sleep time against what the
	 * underlying disk can do, instead of having a static sleep
	 * time.  This is useful for the case where our storage is so
	 * fast that it is more optimal to go ahead and force a flush
	 * and wait for the transaction to be committed than it is to
	 * wait for an arbitrary amount of time for new writers to
	 * join the transaction.  We achieve this by measuring how
	 * long it takes to commit a transaction, and compare it with
	 * how long this transaction has been running, and if run time
	 * < commit time then we sleep for the delta and commit.  This
	 * greatly helps super fast disks that would see slowdowns as
	 * more threads started doing fsyncs.
	 *
	 * But don't do this if this process was the most recent one
	 * to perform a synchronous write.  We do this to detect the
	 * case where a single process is doing a stream of sync
	 * writes.  No point in waiting for joiners in that case.
	 *
	 * Setting max_batch_time to 0 disables this completely.
	 */
	pid = current->pid;
	if (handle->h_sync && journal->j_last_sync_writer != pid &&
	    journal->j_max_batch_time) {
		u64 commit_time, trans_time;

		journal->j_last_sync_writer = pid;

		read_lock(&journal->j_state_lock);
		commit_time = journal->j_average_commit_time;
		read_unlock(&journal->j_state_lock);

		trans_time = ktime_to_ns(ktime_sub(ktime_get(),
						   transaction->t_start_time));

		commit_time = max_t(u64, commit_time,
				    1000*journal->j_min_batch_time);
		commit_time = min_t(u64, commit_time,
				    1000*journal->j_max_batch_time);

		if (trans_time < commit_time) {
			ktime_t expires = ktime_add_ns(ktime_get(),
						       commit_time);
			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_hrtimeout(&expires, HRTIMER_MODE_ABS);
		}
	}

	if (handle->h_sync)
		transaction->t_synchronous_commit = 1;
	current->journal_info = NULL;
	atomic_sub(handle->h_buffer_credits,
		   &transaction->t_outstanding_credits);

	/*
	 * If the handle is marked SYNC, we need to set another commit
	 * going!  We also want to force a commit if the current
	 * transaction is occupying too much of the log, or if the
	 * transaction is too old now.
	 */
	if (handle->h_sync ||
	    (atomic_read(&transaction->t_outstanding_credits) >
	     journal->j_max_transaction_buffers) ||
	    time_after_eq(jiffies, transaction->t_expires)) {
		/* Do this even for aborted journals: an abort still
		 * completes the commit thread, it just doesn't write
		 * anything to disk. */

		jbd_debug(2, "transaction too old, requesting commit for "
					"handle %p\n", handle);
		/* This is non-blocking */
		jbd2_log_start_commit(journal, transaction->t_tid);

		/*
		 * Special case: JBD2_SYNC synchronous updates require us
		 * to wait for the commit to complete.
		 */
		if (handle->h_sync && !(current->flags & PF_MEMALLOC))
			wait_for_commit = 1;
	}

	/*
	 * Once we drop t_updates, if it goes to zero the transaction
	 * could start committing on us and eventually disappear.  So
	 * once we do this, we must not dereference transaction
	 * pointer again.
	 */
	tid = transaction->t_tid;
	if (atomic_dec_and_test(&transaction->t_updates)) {
		wake_up(&journal->j_wait_updates);
		if (journal->j_barrier_count)
			wake_up(&journal->j_wait_transaction_locked);
	}

	rwsem_release(&journal->j_trans_commit_map, 1, _THIS_IP_);

	if (wait_for_commit)
		err = jbd2_log_wait_commit(journal, tid);

	if (handle->h_rsv_handle)
		jbd2_journal_free_reserved(handle->h_rsv_handle);
free_and_exit:
	/*
	 * Scope of the GFP_NOFS context is over here and so we can restore the
	 * original alloc context.
	 */
	memalloc_nofs_restore(handle->saved_alloc_context);
	jbd2_free_handle(handle);
	return err;
}
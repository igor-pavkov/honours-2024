static int add_transaction_credits(journal_t *journal, int blocks,
				   int rsv_blocks)
{
	transaction_t *t = journal->j_running_transaction;
	int needed;
	int total = blocks + rsv_blocks;

	/*
	 * If the current transaction is locked down for commit, wait
	 * for the lock to be released.
	 */
	if (t->t_state == T_LOCKED) {
		wait_transaction_locked(journal);
		return 1;
	}

	/*
	 * If there is not enough space left in the log to write all
	 * potential buffers requested by this operation, we need to
	 * stall pending a log checkpoint to free some more log space.
	 */
	needed = atomic_add_return(total, &t->t_outstanding_credits);
	if (needed > journal->j_max_transaction_buffers) {
		/*
		 * If the current transaction is already too large,
		 * then start to commit it: we can then go back and
		 * attach this handle to a new transaction.
		 */
		atomic_sub(total, &t->t_outstanding_credits);

		/*
		 * Is the number of reserved credits in the current transaction too
		 * big to fit this handle? Wait until reserved credits are freed.
		 */
		if (atomic_read(&journal->j_reserved_credits) + total >
		    journal->j_max_transaction_buffers) {
			read_unlock(&journal->j_state_lock);
			jbd2_might_wait_for_commit(journal);
			wait_event(journal->j_wait_reserved,
				   atomic_read(&journal->j_reserved_credits) + total <=
				   journal->j_max_transaction_buffers);
			return 1;
		}

		wait_transaction_locked(journal);
		return 1;
	}

	/*
	 * The commit code assumes that it can get enough log space
	 * without forcing a checkpoint.  This is *critical* for
	 * correctness: a checkpoint of a buffer which is also
	 * associated with a committing transaction creates a deadlock,
	 * so commit simply cannot force through checkpoints.
	 *
	 * We must therefore ensure the necessary space in the journal
	 * *before* starting to dirty potentially checkpointed buffers
	 * in the new transaction.
	 */
	if (jbd2_log_space_left(journal) < jbd2_space_needed(journal)) {
		atomic_sub(total, &t->t_outstanding_credits);
		read_unlock(&journal->j_state_lock);
		jbd2_might_wait_for_commit(journal);
		write_lock(&journal->j_state_lock);
		if (jbd2_log_space_left(journal) < jbd2_space_needed(journal))
			__jbd2_log_wait_for_space(journal);
		write_unlock(&journal->j_state_lock);
		return 1;
	}

	/* No reservation? We are done... */
	if (!rsv_blocks)
		return 0;

	needed = atomic_add_return(rsv_blocks, &journal->j_reserved_credits);
	/* We allow at most half of a transaction to be reserved */
	if (needed > journal->j_max_transaction_buffers / 2) {
		sub_reserved_credits(journal, rsv_blocks);
		atomic_sub(total, &t->t_outstanding_credits);
		read_unlock(&journal->j_state_lock);
		jbd2_might_wait_for_commit(journal);
		wait_event(journal->j_wait_reserved,
			 atomic_read(&journal->j_reserved_credits) + rsv_blocks
			 <= journal->j_max_transaction_buffers / 2);
		return 1;
	}
	return 0;
}
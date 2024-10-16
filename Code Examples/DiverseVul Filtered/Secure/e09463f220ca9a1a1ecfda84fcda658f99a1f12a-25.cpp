static int jbd2_journal_file_inode(handle_t *handle, struct jbd2_inode *jinode,
				   unsigned long flags)
{
	transaction_t *transaction = handle->h_transaction;
	journal_t *journal;

	if (is_handle_aborted(handle))
		return -EROFS;
	journal = transaction->t_journal;

	jbd_debug(4, "Adding inode %lu, tid:%d\n", jinode->i_vfs_inode->i_ino,
			transaction->t_tid);

	/*
	 * First check whether inode isn't already on the transaction's
	 * lists without taking the lock. Note that this check is safe
	 * without the lock as we cannot race with somebody removing inode
	 * from the transaction. The reason is that we remove inode from the
	 * transaction only in journal_release_jbd_inode() and when we commit
	 * the transaction. We are guarded from the first case by holding
	 * a reference to the inode. We are safe against the second case
	 * because if jinode->i_transaction == transaction, commit code
	 * cannot touch the transaction because we hold reference to it,
	 * and if jinode->i_next_transaction == transaction, commit code
	 * will only file the inode where we want it.
	 */
	if ((jinode->i_transaction == transaction ||
	    jinode->i_next_transaction == transaction) &&
	    (jinode->i_flags & flags) == flags)
		return 0;

	spin_lock(&journal->j_list_lock);
	jinode->i_flags |= flags;
	/* Is inode already attached where we need it? */
	if (jinode->i_transaction == transaction ||
	    jinode->i_next_transaction == transaction)
		goto done;

	/*
	 * We only ever set this variable to 1 so the test is safe. Since
	 * t_need_data_flush is likely to be set, we do the test to save some
	 * cacheline bouncing
	 */
	if (!transaction->t_need_data_flush)
		transaction->t_need_data_flush = 1;
	/* On some different transaction's list - should be
	 * the committing one */
	if (jinode->i_transaction) {
		J_ASSERT(jinode->i_next_transaction == NULL);
		J_ASSERT(jinode->i_transaction ==
					journal->j_committing_transaction);
		jinode->i_next_transaction = transaction;
		goto done;
	}
	/* Not on any transaction list... */
	J_ASSERT(!jinode->i_next_transaction);
	jinode->i_transaction = transaction;
	list_add(&jinode->i_list, &transaction->t_inode_list);
done:
	spin_unlock(&journal->j_list_lock);

	return 0;
}
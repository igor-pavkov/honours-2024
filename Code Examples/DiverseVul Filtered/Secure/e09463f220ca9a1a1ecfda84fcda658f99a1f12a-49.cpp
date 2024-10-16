static bool jbd2_write_access_granted(handle_t *handle, struct buffer_head *bh,
							bool undo)
{
	struct journal_head *jh;
	bool ret = false;

	/* Dirty buffers require special handling... */
	if (buffer_dirty(bh))
		return false;

	/*
	 * RCU protects us from dereferencing freed pages. So the checks we do
	 * are guaranteed not to oops. However the jh slab object can get freed
	 * & reallocated while we work with it. So we have to be careful. When
	 * we see jh attached to the running transaction, we know it must stay
	 * so until the transaction is committed. Thus jh won't be freed and
	 * will be attached to the same bh while we run.  However it can
	 * happen jh gets freed, reallocated, and attached to the transaction
	 * just after we get pointer to it from bh. So we have to be careful
	 * and recheck jh still belongs to our bh before we return success.
	 */
	rcu_read_lock();
	if (!buffer_jbd(bh))
		goto out;
	/* This should be bh2jh() but that doesn't work with inline functions */
	jh = READ_ONCE(bh->b_private);
	if (!jh)
		goto out;
	/* For undo access buffer must have data copied */
	if (undo && !jh->b_committed_data)
		goto out;
	if (jh->b_transaction != handle->h_transaction &&
	    jh->b_next_transaction != handle->h_transaction)
		goto out;
	/*
	 * There are two reasons for the barrier here:
	 * 1) Make sure to fetch b_bh after we did previous checks so that we
	 * detect when jh went through free, realloc, attach to transaction
	 * while we were checking. Paired with implicit barrier in that path.
	 * 2) So that access to bh done after jbd2_write_access_granted()
	 * doesn't get reordered and see inconsistent state of concurrent
	 * do_get_write_access().
	 */
	smp_mb();
	if (unlikely(jh->b_bh != bh))
		goto out;
	ret = true;
out:
	rcu_read_unlock();
	return ret;
}
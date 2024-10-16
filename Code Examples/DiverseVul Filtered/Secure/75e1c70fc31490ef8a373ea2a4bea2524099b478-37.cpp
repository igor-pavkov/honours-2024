static void io_destroy(struct kioctx *ioctx)
{
	struct mm_struct *mm = current->mm;
	int was_dead;

	/* delete the entry from the list is someone else hasn't already */
	spin_lock(&mm->ioctx_lock);
	was_dead = ioctx->dead;
	ioctx->dead = 1;
	hlist_del_rcu(&ioctx->list);
	spin_unlock(&mm->ioctx_lock);

	dprintk("aio_release(%p)\n", ioctx);
	if (likely(!was_dead))
		put_ioctx(ioctx);	/* twice for the list */

	aio_cancel_all(ioctx);
	wait_for_all_aios(ioctx);

	/*
	 * Wake up any waiters.  The setting of ctx->dead must be seen
	 * by other CPUs at this point.  Right now, we rely on the
	 * locking done by the above calls to ensure this consistency.
	 */
	wake_up(&ioctx->wait);
	put_ioctx(ioctx);	/* once for the lookup */
}
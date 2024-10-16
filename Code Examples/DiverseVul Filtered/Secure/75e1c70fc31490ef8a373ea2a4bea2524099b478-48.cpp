static int __aio_put_req(struct kioctx *ctx, struct kiocb *req)
{
	dprintk(KERN_DEBUG "aio_put(%p): f_count=%ld\n",
		req, atomic_long_read(&req->ki_filp->f_count));

	assert_spin_locked(&ctx->ctx_lock);

	req->ki_users--;
	BUG_ON(req->ki_users < 0);
	if (likely(req->ki_users))
		return 0;
	list_del(&req->ki_list);		/* remove from active_reqs */
	req->ki_cancel = NULL;
	req->ki_retry = NULL;

	/*
	 * Try to optimize the aio and eventfd file* puts, by avoiding to
	 * schedule work in case it is not final fput() time. In normal cases,
	 * we would not be holding the last reference to the file*, so
	 * this function will be executed w/out any aio kthread wakeup.
	 */
	if (unlikely(!fput_atomic(req->ki_filp))) {
		get_ioctx(ctx);
		spin_lock(&fput_lock);
		list_add(&req->ki_list, &fput_head);
		spin_unlock(&fput_lock);
		queue_work(aio_wq, &fput_work);
	} else {
		req->ki_filp = NULL;
		really_put_req(ctx, req);
	}
	return 1;
}
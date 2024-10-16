static void aio_kick_handler(struct work_struct *work)
{
	struct kioctx *ctx = container_of(work, struct kioctx, wq.work);
	mm_segment_t oldfs = get_fs();
	struct mm_struct *mm;
	int requeue;

	set_fs(USER_DS);
	use_mm(ctx->mm);
	spin_lock_irq(&ctx->ctx_lock);
	requeue =__aio_run_iocbs(ctx);
	mm = ctx->mm;
	spin_unlock_irq(&ctx->ctx_lock);
 	unuse_mm(mm);
	set_fs(oldfs);
	/*
	 * we're in a worker thread already, don't use queue_delayed_work,
	 */
	if (requeue)
		queue_delayed_work(aio_wq, &ctx->wq, 0);
}
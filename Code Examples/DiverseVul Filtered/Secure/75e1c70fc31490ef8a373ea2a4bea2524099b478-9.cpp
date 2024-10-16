static inline void aio_run_iocbs(struct kioctx *ctx)
{
	int requeue;

	spin_lock_irq(&ctx->ctx_lock);

	requeue = __aio_run_iocbs(ctx);
	spin_unlock_irq(&ctx->ctx_lock);
	if (requeue)
		aio_queue_work(ctx);
}
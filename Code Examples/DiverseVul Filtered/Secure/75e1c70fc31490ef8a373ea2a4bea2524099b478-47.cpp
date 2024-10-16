static void try_queue_kicked_iocb(struct kiocb *iocb)
{
 	struct kioctx	*ctx = iocb->ki_ctx;
	unsigned long flags;
	int run = 0;

	spin_lock_irqsave(&ctx->ctx_lock, flags);
	/* set this inside the lock so that we can't race with aio_run_iocb()
	 * testing it and putting the iocb on the run list under the lock */
	if (!kiocbTryKick(iocb))
		run = __queue_kicked_iocb(iocb);
	spin_unlock_irqrestore(&ctx->ctx_lock, flags);
	if (run)
		aio_queue_work(ctx);
}
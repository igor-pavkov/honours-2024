static void aio_cancel_all(struct kioctx *ctx)
{
	int (*cancel)(struct kiocb *, struct io_event *);
	struct io_event res;
	spin_lock_irq(&ctx->ctx_lock);
	ctx->dead = 1;
	while (!list_empty(&ctx->active_reqs)) {
		struct list_head *pos = ctx->active_reqs.next;
		struct kiocb *iocb = list_kiocb(pos);
		list_del_init(&iocb->ki_list);
		cancel = iocb->ki_cancel;
		kiocbSetCancelled(iocb);
		if (cancel) {
			iocb->ki_users++;
			spin_unlock_irq(&ctx->ctx_lock);
			cancel(iocb, &res);
			spin_lock_irq(&ctx->ctx_lock);
		}
	}
	spin_unlock_irq(&ctx->ctx_lock);
}
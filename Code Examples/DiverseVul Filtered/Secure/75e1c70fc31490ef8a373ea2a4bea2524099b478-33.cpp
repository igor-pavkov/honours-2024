static void aio_fput_routine(struct work_struct *data)
{
	spin_lock_irq(&fput_lock);
	while (likely(!list_empty(&fput_head))) {
		struct kiocb *req = list_kiocb(fput_head.next);
		struct kioctx *ctx = req->ki_ctx;

		list_del(&req->ki_list);
		spin_unlock_irq(&fput_lock);

		/* Complete the fput(s) */
		if (req->ki_filp != NULL)
			fput(req->ki_filp);

		/* Link the iocb into the context's free list */
		spin_lock_irq(&ctx->ctx_lock);
		really_put_req(ctx, req);
		spin_unlock_irq(&ctx->ctx_lock);

		put_ioctx(ctx);
		spin_lock_irq(&fput_lock);
	}
	spin_unlock_irq(&fput_lock);
}
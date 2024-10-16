static void __put_ioctx(struct kioctx *ctx)
{
	BUG_ON(ctx->reqs_active);

	cancel_delayed_work(&ctx->wq);
	cancel_work_sync(&ctx->wq.work);
	aio_free_ring(ctx);
	mmdrop(ctx->mm);
	ctx->mm = NULL;
	pr_debug("__put_ioctx: freeing %p\n", ctx);
	call_rcu(&ctx->rcu_head, ctx_rcu_free);
}
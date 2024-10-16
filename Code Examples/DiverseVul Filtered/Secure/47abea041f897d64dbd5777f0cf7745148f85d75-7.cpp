static int __io_async_cancel(struct io_cancel_data *cd,
			     struct io_uring_task *tctx,
			     unsigned int issue_flags)
{
	bool all = cd->flags & (IORING_ASYNC_CANCEL_ALL|IORING_ASYNC_CANCEL_ANY);
	struct io_ring_ctx *ctx = cd->ctx;
	struct io_tctx_node *node;
	int ret, nr = 0;

	do {
		ret = io_try_cancel(tctx, cd, issue_flags);
		if (ret == -ENOENT)
			break;
		if (!all)
			return ret;
		nr++;
	} while (1);

	/* slow path, try all io-wq's */
	io_ring_submit_lock(ctx, issue_flags);
	ret = -ENOENT;
	list_for_each_entry(node, &ctx->tctx_list, ctx_node) {
		struct io_uring_task *tctx = node->task->io_uring;

		ret = io_async_cancel_one(tctx, cd);
		if (ret != -ENOENT) {
			if (!all)
				break;
			nr++;
		}
	}
	io_ring_submit_unlock(ctx, issue_flags);
	return all ? nr : ret;
}
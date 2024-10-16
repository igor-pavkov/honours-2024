static inline int __queue_kicked_iocb(struct kiocb *iocb)
{
	struct kioctx *ctx = iocb->ki_ctx;

	assert_spin_locked(&ctx->ctx_lock);

	if (list_empty(&iocb->ki_run_list)) {
		list_add_tail(&iocb->ki_run_list,
			&ctx->run_list);
		return 1;
	}
	return 0;
}
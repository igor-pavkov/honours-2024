static int io_async_cancel_one(struct io_uring_task *tctx,
			       struct io_cancel_data *cd)
{
	enum io_wq_cancel cancel_ret;
	int ret = 0;
	bool all;

	if (!tctx || !tctx->io_wq)
		return -ENOENT;

	all = cd->flags & (IORING_ASYNC_CANCEL_ALL|IORING_ASYNC_CANCEL_ANY);
	cancel_ret = io_wq_cancel_cb(tctx->io_wq, io_cancel_cb, cd, all);
	switch (cancel_ret) {
	case IO_WQ_CANCEL_OK:
		ret = 0;
		break;
	case IO_WQ_CANCEL_RUNNING:
		ret = -EALREADY;
		break;
	case IO_WQ_CANCEL_NOTFOUND:
		ret = -ENOENT;
		break;
	}

	return ret;
}
static bool io_cancel_cb(struct io_wq_work *work, void *data)
{
	struct io_kiocb *req = container_of(work, struct io_kiocb, work);
	struct io_cancel_data *cd = data;

	if (req->ctx != cd->ctx)
		return false;
	if (cd->flags & IORING_ASYNC_CANCEL_ANY) {
		;
	} else if (cd->flags & IORING_ASYNC_CANCEL_FD) {
		if (req->file != cd->file)
			return false;
	} else {
		if (req->cqe.user_data != cd->data)
			return false;
	}
	if (cd->flags & (IORING_ASYNC_CANCEL_ALL|IORING_ASYNC_CANCEL_ANY)) {
		if (cd->seq == req->work.cancel_seq)
			return false;
		req->work.cancel_seq = cd->seq;
	}
	return true;
}
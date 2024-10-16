int io_async_cancel(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_cancel *cancel = io_kiocb_to_cmd(req, struct io_cancel);
	struct io_cancel_data cd = {
		.ctx	= req->ctx,
		.data	= cancel->addr,
		.flags	= cancel->flags,
		.seq	= atomic_inc_return(&req->ctx->cancel_seq),
	};
	struct io_uring_task *tctx = req->task->io_uring;
	int ret;

	if (cd.flags & IORING_ASYNC_CANCEL_FD) {
		if (req->flags & REQ_F_FIXED_FILE ||
		    cd.flags & IORING_ASYNC_CANCEL_FD_FIXED) {
			req->flags |= REQ_F_FIXED_FILE;
			req->file = io_file_get_fixed(req, cancel->fd,
							issue_flags);
		} else {
			req->file = io_file_get_normal(req, cancel->fd);
		}
		if (!req->file) {
			ret = -EBADF;
			goto done;
		}
		cd.file = req->file;
	}

	ret = __io_async_cancel(&cd, tctx, issue_flags);
done:
	if (ret < 0)
		req_set_fail(req);
	io_req_set_res(req, ret, 0);
	return IOU_OK;
}
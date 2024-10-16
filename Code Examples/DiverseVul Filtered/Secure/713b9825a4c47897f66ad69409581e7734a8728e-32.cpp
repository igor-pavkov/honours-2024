enum io_wq_cancel io_wq_cancel_cb(struct io_wq *wq, work_cancel_fn *cancel,
				  void *data, bool cancel_all)
{
	struct io_cb_cancel_data match = {
		.fn		= cancel,
		.data		= data,
		.cancel_all	= cancel_all,
	};
	int node;

	/*
	 * First check pending list, if we're lucky we can just remove it
	 * from there. CANCEL_OK means that the work is returned as-new,
	 * no completion will be posted for it.
	 */
	for_each_node(node) {
		struct io_wqe *wqe = wq->wqes[node];

		io_wqe_cancel_pending_work(wqe, &match);
		if (match.nr_pending && !match.cancel_all)
			return IO_WQ_CANCEL_OK;
	}

	/*
	 * Now check if a free (going busy) or busy worker has the work
	 * currently running. If we find it there, we'll return CANCEL_RUNNING
	 * as an indication that we attempt to signal cancellation. The
	 * completion will run normally in this case.
	 */
	for_each_node(node) {
		struct io_wqe *wqe = wq->wqes[node];

		io_wqe_cancel_running_work(wqe, &match);
		if (match.nr_running && !match.cancel_all)
			return IO_WQ_CANCEL_RUNNING;
	}

	if (match.nr_running)
		return IO_WQ_CANCEL_RUNNING;
	if (match.nr_pending)
		return IO_WQ_CANCEL_OK;
	return IO_WQ_CANCEL_NOTFOUND;
}
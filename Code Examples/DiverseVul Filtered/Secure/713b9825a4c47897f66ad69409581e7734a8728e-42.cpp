static inline struct io_wqe_acct *io_wqe_get_acct(struct io_worker *worker)
{
	return io_get_acct(worker->wqe, worker->flags & IO_WORKER_F_BOUND);
}
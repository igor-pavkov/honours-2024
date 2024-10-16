static inline struct io_wqe_acct *io_work_get_acct(struct io_wqe *wqe,
						   struct io_wq_work *work)
{
	return io_get_acct(wqe, !(work->flags & IO_WQ_WORK_UNBOUND));
}
static inline struct io_wqe_acct *io_get_acct(struct io_wqe *wqe, bool bound)
{
	return &wqe->acct[bound ? IO_WQ_ACCT_BOUND : IO_WQ_ACCT_UNBOUND];
}
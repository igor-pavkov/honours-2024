static void io_wqe_inc_running(struct io_worker *worker)
{
	struct io_wqe_acct *acct = io_wqe_get_acct(worker);

	atomic_inc(&acct->nr_running);
}
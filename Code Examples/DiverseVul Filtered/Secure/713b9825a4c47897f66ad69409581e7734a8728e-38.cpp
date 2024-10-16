static inline bool io_acct_run_queue(struct io_wqe_acct *acct)
{
	if (!wq_list_empty(&acct->work_list) &&
	    !test_bit(IO_ACCT_STALLED_BIT, &acct->flags))
		return true;
	return false;
}
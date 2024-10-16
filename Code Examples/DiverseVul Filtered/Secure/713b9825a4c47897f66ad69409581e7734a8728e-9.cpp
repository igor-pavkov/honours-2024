static void io_wqe_cancel_pending_work(struct io_wqe *wqe,
				       struct io_cb_cancel_data *match)
{
	int i;
retry:
	raw_spin_lock(&wqe->lock);
	for (i = 0; i < IO_WQ_ACCT_NR; i++) {
		struct io_wqe_acct *acct = io_get_acct(wqe, i == 0);

		if (io_acct_cancel_pending_work(wqe, acct, match)) {
			if (match->cancel_all)
				goto retry;
			return;
		}
	}
	raw_spin_unlock(&wqe->lock);
}
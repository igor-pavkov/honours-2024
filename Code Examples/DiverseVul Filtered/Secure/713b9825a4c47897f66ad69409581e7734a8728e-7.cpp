static int io_wqe_hash_wake(struct wait_queue_entry *wait, unsigned mode,
			    int sync, void *key)
{
	struct io_wqe *wqe = container_of(wait, struct io_wqe, wait);
	int i;

	list_del_init(&wait->entry);

	rcu_read_lock();
	for (i = 0; i < IO_WQ_ACCT_NR; i++) {
		struct io_wqe_acct *acct = &wqe->acct[i];

		if (test_and_clear_bit(IO_ACCT_STALLED_BIT, &acct->flags))
			io_wqe_activate_free_worker(wqe, acct);
	}
	rcu_read_unlock();
	return 1;
}
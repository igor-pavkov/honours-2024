int io_wq_max_workers(struct io_wq *wq, int *new_count)
{
	int i, node, prev = 0;

	for (i = 0; i < 2; i++) {
		if (new_count[i] > task_rlimit(current, RLIMIT_NPROC))
			new_count[i] = task_rlimit(current, RLIMIT_NPROC);
	}

	rcu_read_lock();
	for_each_node(node) {
		struct io_wqe_acct *acct;

		for (i = 0; i < IO_WQ_ACCT_NR; i++) {
			acct = &wq->wqes[node]->acct[i];
			prev = max_t(int, acct->max_workers, prev);
			if (new_count[i])
				acct->max_workers = new_count[i];
			new_count[i] = prev;
		}
	}
	rcu_read_unlock();
	return 0;
}
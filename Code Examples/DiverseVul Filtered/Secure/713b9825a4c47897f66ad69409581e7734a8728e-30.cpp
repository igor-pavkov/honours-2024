static void io_wqe_insert_work(struct io_wqe *wqe, struct io_wq_work *work)
{
	struct io_wqe_acct *acct = io_work_get_acct(wqe, work);
	unsigned int hash;
	struct io_wq_work *tail;

	if (!io_wq_is_hashed(work)) {
append:
		wq_list_add_tail(&work->list, &acct->work_list);
		return;
	}

	hash = io_get_work_hash(work);
	tail = wqe->hash_tail[hash];
	wqe->hash_tail[hash] = work;
	if (!tail)
		goto append;

	wq_list_add_after(&work->list, &tail->list, &acct->work_list);
}
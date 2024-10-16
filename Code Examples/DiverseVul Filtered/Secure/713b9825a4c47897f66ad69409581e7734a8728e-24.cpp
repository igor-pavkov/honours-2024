static void create_worker_cb(struct callback_head *cb)
{
	struct io_worker *worker;
	struct io_wq *wq;
	struct io_wqe *wqe;
	struct io_wqe_acct *acct;
	bool do_create = false;

	worker = container_of(cb, struct io_worker, create_work);
	wqe = worker->wqe;
	wq = wqe->wq;
	acct = &wqe->acct[worker->create_index];
	raw_spin_lock(&wqe->lock);
	if (acct->nr_workers < acct->max_workers) {
		acct->nr_workers++;
		do_create = true;
	}
	raw_spin_unlock(&wqe->lock);
	if (do_create) {
		create_io_worker(wq, wqe, worker->create_index);
	} else {
		atomic_dec(&acct->nr_running);
		io_worker_ref_put(wq);
	}
	clear_bit_unlock(0, &worker->create_state);
	io_worker_release(worker);
}
static void create_worker_cont(struct callback_head *cb)
{
	struct io_worker *worker;
	struct task_struct *tsk;
	struct io_wqe *wqe;

	worker = container_of(cb, struct io_worker, create_work);
	clear_bit_unlock(0, &worker->create_state);
	wqe = worker->wqe;
	tsk = create_io_thread(io_wqe_worker, worker, wqe->node);
	if (!IS_ERR(tsk)) {
		io_init_new_worker(wqe, worker, tsk);
		io_worker_release(worker);
		return;
	} else if (!io_should_retry_thread(PTR_ERR(tsk))) {
		struct io_wqe_acct *acct = io_wqe_get_acct(worker);

		atomic_dec(&acct->nr_running);
		raw_spin_lock(&wqe->lock);
		acct->nr_workers--;
		if (!acct->nr_workers) {
			struct io_cb_cancel_data match = {
				.fn		= io_wq_work_match_all,
				.cancel_all	= true,
			};

			while (io_acct_cancel_pending_work(wqe, acct, &match))
				raw_spin_lock(&wqe->lock);
		}
		raw_spin_unlock(&wqe->lock);
		io_worker_ref_put(wqe->wq);
		return;
	}

	/* re-create attempts grab a new worker ref, drop the existing one */
	io_worker_release(worker);
	schedule_work(&worker->work);
}
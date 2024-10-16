static bool create_io_worker(struct io_wq *wq, struct io_wqe *wqe, int index)
{
	struct io_wqe_acct *acct = &wqe->acct[index];
	struct io_worker *worker;
	struct task_struct *tsk;

	__set_current_state(TASK_RUNNING);

	worker = kzalloc_node(sizeof(*worker), GFP_KERNEL, wqe->node);
	if (!worker) {
fail:
		atomic_dec(&acct->nr_running);
		raw_spin_lock(&wqe->lock);
		acct->nr_workers--;
		raw_spin_unlock(&wqe->lock);
		io_worker_ref_put(wq);
		return false;
	}

	refcount_set(&worker->ref, 1);
	worker->wqe = wqe;
	spin_lock_init(&worker->lock);
	init_completion(&worker->ref_done);

	if (index == IO_WQ_ACCT_BOUND)
		worker->flags |= IO_WORKER_F_BOUND;

	tsk = create_io_thread(io_wqe_worker, worker, wqe->node);
	if (!IS_ERR(tsk)) {
		io_init_new_worker(wqe, worker, tsk);
	} else if (!io_should_retry_thread(PTR_ERR(tsk))) {
		goto fail;
	} else {
		INIT_WORK(&worker->work, io_workqueue_create);
		schedule_work(&worker->work);
	}

	return true;
}
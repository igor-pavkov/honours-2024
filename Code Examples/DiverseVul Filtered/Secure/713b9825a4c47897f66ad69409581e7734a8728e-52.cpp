static void io_wq_exit_workers(struct io_wq *wq)
{
	struct callback_head *cb;
	int node;

	if (!wq->task)
		return;

	while ((cb = task_work_cancel_match(wq->task, io_task_work_match, wq)) != NULL) {
		struct io_worker *worker;

		worker = container_of(cb, struct io_worker, create_work);
		atomic_dec(&worker->wqe->acct[worker->create_index].nr_running);
		io_worker_ref_put(wq);
		clear_bit_unlock(0, &worker->create_state);
		io_worker_release(worker);
	}

	rcu_read_lock();
	for_each_node(node) {
		struct io_wqe *wqe = wq->wqes[node];

		io_wq_for_each_worker(wqe, io_wq_worker_wake, NULL);
	}
	rcu_read_unlock();
	io_worker_ref_put(wq);
	wait_for_completion(&wq->worker_done);

	for_each_node(node) {
		spin_lock_irq(&wq->hash->wait.lock);
		list_del_init(&wq->wqes[node]->wait.entry);
		spin_unlock_irq(&wq->hash->wait.lock);
	}
	put_task_struct(wq->task);
	wq->task = NULL;
}
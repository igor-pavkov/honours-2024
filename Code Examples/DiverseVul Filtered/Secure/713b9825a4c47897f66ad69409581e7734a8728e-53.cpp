static void io_workqueue_create(struct work_struct *work)
{
	struct io_worker *worker = container_of(work, struct io_worker, work);
	struct io_wqe_acct *acct = io_wqe_get_acct(worker);

	if (!io_queue_worker_create(worker, acct, create_worker_cont)) {
		clear_bit_unlock(0, &worker->create_state);
		io_worker_release(worker);
	}
}
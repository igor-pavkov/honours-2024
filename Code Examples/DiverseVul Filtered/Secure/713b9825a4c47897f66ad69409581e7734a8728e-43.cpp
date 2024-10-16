void io_wq_worker_sleeping(struct task_struct *tsk)
{
	struct io_worker *worker = tsk->pf_io_worker;

	if (!worker)
		return;
	if (!(worker->flags & IO_WORKER_F_UP))
		return;
	if (!(worker->flags & IO_WORKER_F_RUNNING))
		return;

	worker->flags &= ~IO_WORKER_F_RUNNING;

	raw_spin_lock(&worker->wqe->lock);
	io_wqe_dec_running(worker);
	raw_spin_unlock(&worker->wqe->lock);
}
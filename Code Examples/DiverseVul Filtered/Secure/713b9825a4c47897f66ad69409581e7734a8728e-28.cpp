static void io_assign_current_work(struct io_worker *worker,
				   struct io_wq_work *work)
{
	if (work) {
		io_flush_signals();
		cond_resched();
	}

	spin_lock(&worker->lock);
	worker->cur_work = work;
	spin_unlock(&worker->lock);
}
static int io_wqe_worker(void *data)
{
	struct io_worker *worker = data;
	struct io_wqe_acct *acct = io_wqe_get_acct(worker);
	struct io_wqe *wqe = worker->wqe;
	struct io_wq *wq = wqe->wq;
	bool last_timeout = false;
	char buf[TASK_COMM_LEN];

	worker->flags |= (IO_WORKER_F_UP | IO_WORKER_F_RUNNING);

	snprintf(buf, sizeof(buf), "iou-wrk-%d", wq->task->pid);
	set_task_comm(current, buf);

	while (!test_bit(IO_WQ_BIT_EXIT, &wq->state)) {
		long ret;

		set_current_state(TASK_INTERRUPTIBLE);
loop:
		raw_spin_lock(&wqe->lock);
		if (io_acct_run_queue(acct)) {
			io_worker_handle_work(worker);
			goto loop;
		}
		/* timed out, exit unless we're the last worker */
		if (last_timeout && acct->nr_workers > 1) {
			raw_spin_unlock(&wqe->lock);
			__set_current_state(TASK_RUNNING);
			break;
		}
		last_timeout = false;
		__io_worker_idle(wqe, worker);
		raw_spin_unlock(&wqe->lock);
		if (io_flush_signals())
			continue;
		ret = schedule_timeout(WORKER_IDLE_TIMEOUT);
		if (signal_pending(current)) {
			struct ksignal ksig;

			if (!get_signal(&ksig))
				continue;
			if (fatal_signal_pending(current))
				break;
			continue;
		}
		last_timeout = !ret;
	}

	if (test_bit(IO_WQ_BIT_EXIT, &wq->state)) {
		raw_spin_lock(&wqe->lock);
		io_worker_handle_work(worker);
	}

	io_worker_exit(worker);
	return 0;
}
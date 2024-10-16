static void io_init_new_worker(struct io_wqe *wqe, struct io_worker *worker,
			       struct task_struct *tsk)
{
	tsk->pf_io_worker = worker;
	worker->task = tsk;
	set_cpus_allowed_ptr(tsk, wqe->cpu_mask);
	tsk->flags |= PF_NO_SETAFFINITY;

	raw_spin_lock(&wqe->lock);
	hlist_nulls_add_head_rcu(&worker->nulls_node, &wqe->free_list);
	list_add_tail_rcu(&worker->all_list, &wqe->all_list);
	worker->flags |= IO_WORKER_F_FREE;
	raw_spin_unlock(&wqe->lock);
	wake_up_new_task(tsk);
}
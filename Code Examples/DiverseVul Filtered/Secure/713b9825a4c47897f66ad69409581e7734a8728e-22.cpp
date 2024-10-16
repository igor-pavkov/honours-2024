static void io_worker_exit(struct io_worker *worker)
{
	struct io_wqe *wqe = worker->wqe;
	struct io_wqe_acct *acct = io_wqe_get_acct(worker);

	if (refcount_dec_and_test(&worker->ref))
		complete(&worker->ref_done);
	wait_for_completion(&worker->ref_done);

	raw_spin_lock(&wqe->lock);
	if (worker->flags & IO_WORKER_F_FREE)
		hlist_nulls_del_rcu(&worker->nulls_node);
	list_del_rcu(&worker->all_list);
	acct->nr_workers--;
	preempt_disable();
	io_wqe_dec_running(worker);
	worker->flags = 0;
	current->flags &= ~PF_IO_WORKER;
	preempt_enable();
	raw_spin_unlock(&wqe->lock);

	kfree_rcu(worker, rcu);
	io_worker_ref_put(wqe->wq);
	do_exit(0);
}
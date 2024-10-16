int io_wq_cpu_affinity(struct io_wq *wq, cpumask_var_t mask)
{
	int i;

	rcu_read_lock();
	for_each_node(i) {
		struct io_wqe *wqe = wq->wqes[i];

		if (mask)
			cpumask_copy(wqe->cpu_mask, mask);
		else
			cpumask_copy(wqe->cpu_mask, cpumask_of_node(i));
	}
	rcu_read_unlock();
	return 0;
}
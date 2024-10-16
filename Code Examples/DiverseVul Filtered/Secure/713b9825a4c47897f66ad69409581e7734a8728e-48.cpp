struct io_wq *io_wq_create(unsigned bounded, struct io_wq_data *data)
{
	int ret, node, i;
	struct io_wq *wq;

	if (WARN_ON_ONCE(!data->free_work || !data->do_work))
		return ERR_PTR(-EINVAL);
	if (WARN_ON_ONCE(!bounded))
		return ERR_PTR(-EINVAL);

	wq = kzalloc(struct_size(wq, wqes, nr_node_ids), GFP_KERNEL);
	if (!wq)
		return ERR_PTR(-ENOMEM);
	ret = cpuhp_state_add_instance_nocalls(io_wq_online, &wq->cpuhp_node);
	if (ret)
		goto err_wq;

	refcount_inc(&data->hash->refs);
	wq->hash = data->hash;
	wq->free_work = data->free_work;
	wq->do_work = data->do_work;

	ret = -ENOMEM;
	for_each_node(node) {
		struct io_wqe *wqe;
		int alloc_node = node;

		if (!node_online(alloc_node))
			alloc_node = NUMA_NO_NODE;
		wqe = kzalloc_node(sizeof(struct io_wqe), GFP_KERNEL, alloc_node);
		if (!wqe)
			goto err;
		if (!alloc_cpumask_var(&wqe->cpu_mask, GFP_KERNEL))
			goto err;
		cpumask_copy(wqe->cpu_mask, cpumask_of_node(node));
		wq->wqes[node] = wqe;
		wqe->node = alloc_node;
		wqe->acct[IO_WQ_ACCT_BOUND].max_workers = bounded;
		wqe->acct[IO_WQ_ACCT_UNBOUND].max_workers =
					task_rlimit(current, RLIMIT_NPROC);
		INIT_LIST_HEAD(&wqe->wait.entry);
		wqe->wait.func = io_wqe_hash_wake;
		for (i = 0; i < IO_WQ_ACCT_NR; i++) {
			struct io_wqe_acct *acct = &wqe->acct[i];

			acct->index = i;
			atomic_set(&acct->nr_running, 0);
			INIT_WQ_LIST(&acct->work_list);
		}
		wqe->wq = wq;
		raw_spin_lock_init(&wqe->lock);
		INIT_HLIST_NULLS_HEAD(&wqe->free_list, 0);
		INIT_LIST_HEAD(&wqe->all_list);
	}

	wq->task = get_task_struct(data->task);
	atomic_set(&wq->worker_refs, 1);
	init_completion(&wq->worker_done);
	return wq;
err:
	io_wq_put_hash(data->hash);
	cpuhp_state_remove_instance_nocalls(io_wq_online, &wq->cpuhp_node);
	for_each_node(node) {
		if (!wq->wqes[node])
			continue;
		free_cpumask_var(wq->wqes[node]->cpu_mask);
		kfree(wq->wqes[node]);
	}
err_wq:
	kfree(wq);
	return ERR_PTR(ret);
}
static void io_wq_destroy(struct io_wq *wq)
{
	int node;

	cpuhp_state_remove_instance_nocalls(io_wq_online, &wq->cpuhp_node);

	for_each_node(node) {
		struct io_wqe *wqe = wq->wqes[node];
		struct io_cb_cancel_data match = {
			.fn		= io_wq_work_match_all,
			.cancel_all	= true,
		};
		io_wqe_cancel_pending_work(wqe, &match);
		free_cpumask_var(wqe->cpu_mask);
		kfree(wqe);
	}
	io_wq_put_hash(wq->hash);
	kfree(wq);
}
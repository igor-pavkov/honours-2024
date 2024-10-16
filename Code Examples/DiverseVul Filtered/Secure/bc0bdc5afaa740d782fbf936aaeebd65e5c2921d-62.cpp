static void enqueue_resolve_addr_work(struct cma_work *work,
				      struct rdma_id_private *id_priv)
{
	/* Balances with cma_id_put() in cma_work_handler */
	cma_id_get(id_priv);

	work->id = id_priv;
	INIT_WORK(&work->work, cma_work_handler);
	work->old_state = RDMA_CM_ADDR_QUERY;
	work->new_state = RDMA_CM_ADDR_RESOLVED;
	work->event.event = RDMA_CM_EVENT_ADDR_RESOLVED;

	queue_work(cma_wq, &work->work);
}
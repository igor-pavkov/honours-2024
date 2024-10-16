static int cma_resolve_iw_route(struct rdma_id_private *id_priv)
{
	struct cma_work *work;

	work = kzalloc(sizeof *work, GFP_KERNEL);
	if (!work)
		return -ENOMEM;

	cma_init_resolve_route_work(work, id_priv);
	queue_work(cma_wq, &work->work);
	return 0;
}
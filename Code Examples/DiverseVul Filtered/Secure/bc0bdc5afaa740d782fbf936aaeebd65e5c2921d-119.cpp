static void cma_query_handler(int status, struct sa_path_rec *path_rec,
			      void *context)
{
	struct cma_work *work = context;
	struct rdma_route *route;

	route = &work->id->id.route;

	if (!status) {
		route->num_paths = 1;
		*route->path_rec = *path_rec;
	} else {
		work->old_state = RDMA_CM_ROUTE_QUERY;
		work->new_state = RDMA_CM_ADDR_RESOLVED;
		work->event.event = RDMA_CM_EVENT_ROUTE_ERROR;
		work->event.status = status;
		pr_debug_ratelimited("RDMA CM: ROUTE_ERROR: failed to query path. status %d\n",
				     status);
	}

	queue_work(cma_wq, &work->work);
}
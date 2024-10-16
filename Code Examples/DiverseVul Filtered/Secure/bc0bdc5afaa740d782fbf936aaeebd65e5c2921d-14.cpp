static int cma_save_req_info(const struct ib_cm_event *ib_event,
			     struct cma_req_info *req)
{
	const struct ib_cm_req_event_param *req_param =
		&ib_event->param.req_rcvd;
	const struct ib_cm_sidr_req_event_param *sidr_param =
		&ib_event->param.sidr_req_rcvd;

	switch (ib_event->event) {
	case IB_CM_REQ_RECEIVED:
		req->device	= req_param->listen_id->device;
		req->port	= req_param->port;
		memcpy(&req->local_gid, &req_param->primary_path->sgid,
		       sizeof(req->local_gid));
		req->has_gid	= true;
		req->service_id = req_param->primary_path->service_id;
		req->pkey	= be16_to_cpu(req_param->primary_path->pkey);
		if (req->pkey != req_param->bth_pkey)
			pr_warn_ratelimited("RDMA CMA: got different BTH P_Key (0x%x) and primary path P_Key (0x%x)\n"
					    "RDMA CMA: in the future this may cause the request to be dropped\n",
					    req_param->bth_pkey, req->pkey);
		break;
	case IB_CM_SIDR_REQ_RECEIVED:
		req->device	= sidr_param->listen_id->device;
		req->port	= sidr_param->port;
		req->has_gid	= false;
		req->service_id	= sidr_param->service_id;
		req->pkey	= sidr_param->pkey;
		if (req->pkey != sidr_param->bth_pkey)
			pr_warn_ratelimited("RDMA CMA: got different BTH P_Key (0x%x) and SIDR request payload P_Key (0x%x)\n"
					    "RDMA CMA: in the future this may cause the request to be dropped\n",
					    sidr_param->bth_pkey, req->pkey);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
static struct net_device *cma_get_net_dev(const struct ib_cm_event *ib_event,
					  struct cma_req_info *req)
{
	struct sockaddr *listen_addr =
			(struct sockaddr *)&req->listen_addr_storage;
	struct sockaddr *src_addr = (struct sockaddr *)&req->src_addr_storage;
	struct net_device *net_dev;
	const union ib_gid *gid = req->has_gid ? &req->local_gid : NULL;
	int err;

	err = cma_save_ip_info(listen_addr, src_addr, ib_event,
			       req->service_id);
	if (err)
		return ERR_PTR(err);

	if (rdma_protocol_roce(req->device, req->port))
		net_dev = roce_get_net_dev_by_cm_event(ib_event);
	else
		net_dev = ib_get_net_dev_by_params(req->device, req->port,
						   req->pkey,
						   gid, listen_addr);
	if (!net_dev)
		return ERR_PTR(-ENODEV);

	return net_dev;
}
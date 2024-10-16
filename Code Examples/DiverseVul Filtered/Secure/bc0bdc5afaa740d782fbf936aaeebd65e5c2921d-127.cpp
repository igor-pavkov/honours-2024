static void cma_make_mc_event(int status, struct rdma_id_private *id_priv,
			      struct ib_sa_multicast *multicast,
			      struct rdma_cm_event *event,
			      struct cma_multicast *mc)
{
	struct rdma_dev_addr *dev_addr;
	enum ib_gid_type gid_type;
	struct net_device *ndev;

	if (!status)
		status = cma_set_qkey(id_priv, be32_to_cpu(multicast->rec.qkey));
	else
		pr_debug_ratelimited("RDMA CM: MULTICAST_ERROR: failed to join multicast. status %d\n",
				     status);

	event->status = status;
	event->param.ud.private_data = mc->context;
	if (status) {
		event->event = RDMA_CM_EVENT_MULTICAST_ERROR;
		return;
	}

	dev_addr = &id_priv->id.route.addr.dev_addr;
	ndev = dev_get_by_index(dev_addr->net, dev_addr->bound_dev_if);
	gid_type =
		id_priv->cma_dev
			->default_gid_type[id_priv->id.port_num -
					   rdma_start_port(
						   id_priv->cma_dev->device)];

	event->event = RDMA_CM_EVENT_MULTICAST_JOIN;
	if (ib_init_ah_from_mcmember(id_priv->id.device, id_priv->id.port_num,
				     &multicast->rec, ndev, gid_type,
				     &event->param.ud.ah_attr)) {
		event->event = RDMA_CM_EVENT_MULTICAST_ERROR;
		goto out;
	}

	event->param.ud.qp_num = 0xFFFFFF;
	event->param.ud.qkey = be32_to_cpu(multicast->rec.qkey);

out:
	if (ndev)
		dev_put(ndev);
}
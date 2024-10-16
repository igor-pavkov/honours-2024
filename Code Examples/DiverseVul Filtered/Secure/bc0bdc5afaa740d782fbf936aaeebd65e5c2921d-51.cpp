static int cma_resolve_iboe_route(struct rdma_id_private *id_priv)
{
	struct rdma_route *route = &id_priv->id.route;
	struct rdma_addr *addr = &route->addr;
	struct cma_work *work;
	int ret;
	struct net_device *ndev;

	u8 default_roce_tos = id_priv->cma_dev->default_roce_tos[id_priv->id.port_num -
					rdma_start_port(id_priv->cma_dev->device)];
	u8 tos;

	mutex_lock(&id_priv->qp_mutex);
	tos = id_priv->tos_set ? id_priv->tos : default_roce_tos;
	mutex_unlock(&id_priv->qp_mutex);

	work = kzalloc(sizeof *work, GFP_KERNEL);
	if (!work)
		return -ENOMEM;

	route->path_rec = kzalloc(sizeof *route->path_rec, GFP_KERNEL);
	if (!route->path_rec) {
		ret = -ENOMEM;
		goto err1;
	}

	route->num_paths = 1;

	ndev = cma_iboe_set_path_rec_l2_fields(id_priv);
	if (!ndev) {
		ret = -ENODEV;
		goto err2;
	}

	rdma_ip2gid((struct sockaddr *)&id_priv->id.route.addr.src_addr,
		    &route->path_rec->sgid);
	rdma_ip2gid((struct sockaddr *)&id_priv->id.route.addr.dst_addr,
		    &route->path_rec->dgid);

	if (((struct sockaddr *)&id_priv->id.route.addr.dst_addr)->sa_family != AF_IB)
		/* TODO: get the hoplimit from the inet/inet6 device */
		route->path_rec->hop_limit = addr->dev_addr.hoplimit;
	else
		route->path_rec->hop_limit = 1;
	route->path_rec->reversible = 1;
	route->path_rec->pkey = cpu_to_be16(0xffff);
	route->path_rec->mtu_selector = IB_SA_EQ;
	route->path_rec->sl = iboe_tos_to_sl(ndev, tos);
	route->path_rec->traffic_class = tos;
	route->path_rec->mtu = iboe_get_mtu(ndev->mtu);
	route->path_rec->rate_selector = IB_SA_EQ;
	route->path_rec->rate = iboe_get_rate(ndev);
	dev_put(ndev);
	route->path_rec->packet_life_time_selector = IB_SA_EQ;
	/* In case ACK timeout is set, use this value to calculate
	 * PacketLifeTime.  As per IBTA 12.7.34,
	 * local ACK timeout = (2 * PacketLifeTime + Local CA’s ACK delay).
	 * Assuming a negligible local ACK delay, we can use
	 * PacketLifeTime = local ACK timeout/2
	 * as a reasonable approximation for RoCE networks.
	 */
	mutex_lock(&id_priv->qp_mutex);
	if (id_priv->timeout_set && id_priv->timeout)
		route->path_rec->packet_life_time = id_priv->timeout - 1;
	else
		route->path_rec->packet_life_time = CMA_IBOE_PACKET_LIFETIME;
	mutex_unlock(&id_priv->qp_mutex);

	if (!route->path_rec->mtu) {
		ret = -EINVAL;
		goto err2;
	}

	if (rdma_protocol_roce_udp_encap(id_priv->id.device,
					 id_priv->id.port_num))
		route->path_rec->flow_label =
			cma_get_roce_udp_flow_label(id_priv);

	cma_init_resolve_route_work(work, id_priv);
	queue_work(cma_wq, &work->work);

	return 0;

err2:
	kfree(route->path_rec);
	route->path_rec = NULL;
	route->num_paths = 0;
err1:
	kfree(work);
	return ret;
}
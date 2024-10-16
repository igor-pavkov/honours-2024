static int cma_iboe_join_multicast(struct rdma_id_private *id_priv,
				   struct cma_multicast *mc)
{
	struct rdma_dev_addr *dev_addr = &id_priv->id.route.addr.dev_addr;
	int err = 0;
	struct sockaddr *addr = (struct sockaddr *)&mc->addr;
	struct net_device *ndev = NULL;
	struct ib_sa_multicast ib;
	enum ib_gid_type gid_type;
	bool send_only;

	send_only = mc->join_state == BIT(SENDONLY_FULLMEMBER_JOIN);

	if (cma_zero_addr(addr))
		return -EINVAL;

	gid_type = id_priv->cma_dev->default_gid_type[id_priv->id.port_num -
		   rdma_start_port(id_priv->cma_dev->device)];
	cma_iboe_set_mgid(addr, &ib.rec.mgid, gid_type);

	ib.rec.pkey = cpu_to_be16(0xffff);
	if (id_priv->id.ps == RDMA_PS_UDP)
		ib.rec.qkey = cpu_to_be32(RDMA_UDP_QKEY);

	if (dev_addr->bound_dev_if)
		ndev = dev_get_by_index(dev_addr->net, dev_addr->bound_dev_if);
	if (!ndev)
		return -ENODEV;

	ib.rec.rate = iboe_get_rate(ndev);
	ib.rec.hop_limit = 1;
	ib.rec.mtu = iboe_get_mtu(ndev->mtu);

	if (addr->sa_family == AF_INET) {
		if (gid_type == IB_GID_TYPE_ROCE_UDP_ENCAP) {
			ib.rec.hop_limit = IPV6_DEFAULT_HOPLIMIT;
			if (!send_only) {
				err = cma_igmp_send(ndev, &ib.rec.mgid,
						    true);
			}
		}
	} else {
		if (gid_type == IB_GID_TYPE_ROCE_UDP_ENCAP)
			err = -ENOTSUPP;
	}
	dev_put(ndev);
	if (err || !ib.rec.mtu)
		return err ?: -EINVAL;

	rdma_ip2gid((struct sockaddr *)&id_priv->id.route.addr.src_addr,
		    &ib.rec.port_gid);
	INIT_WORK(&mc->iboe_join.work, cma_iboe_join_work_handler);
	cma_make_mc_event(0, id_priv, &ib, &mc->iboe_join.event, mc);
	queue_work(cma_wq, &mc->iboe_join.work);
	return 0;
}
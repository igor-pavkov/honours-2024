static void destroy_mc(struct rdma_id_private *id_priv,
		       struct cma_multicast *mc)
{
	bool send_only = mc->join_state == BIT(SENDONLY_FULLMEMBER_JOIN);

	if (rdma_cap_ib_mcast(id_priv->id.device, id_priv->id.port_num))
		ib_sa_free_multicast(mc->sa_mc);

	if (rdma_protocol_roce(id_priv->id.device, id_priv->id.port_num)) {
		struct rdma_dev_addr *dev_addr =
			&id_priv->id.route.addr.dev_addr;
		struct net_device *ndev = NULL;

		if (dev_addr->bound_dev_if)
			ndev = dev_get_by_index(dev_addr->net,
						dev_addr->bound_dev_if);
		if (ndev) {
			union ib_gid mgid;

			cma_set_mgid(id_priv, (struct sockaddr *)&mc->addr,
				     &mgid);

			if (!send_only)
				cma_igmp_send(ndev, &mgid, false);

			dev_put(ndev);
		}

		cancel_work_sync(&mc->iboe_join.work);
	}
	kfree(mc);
}
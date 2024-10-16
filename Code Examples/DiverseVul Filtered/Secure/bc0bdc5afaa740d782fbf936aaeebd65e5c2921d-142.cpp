int rdma_join_multicast(struct rdma_cm_id *id, struct sockaddr *addr,
			u8 join_state, void *context)
{
	struct rdma_id_private *id_priv =
		container_of(id, struct rdma_id_private, id);
	struct cma_multicast *mc;
	int ret;

	/* Not supported for kernel QPs */
	if (WARN_ON(id->qp))
		return -EINVAL;

	/* ULP is calling this wrong. */
	if (!id->device || (READ_ONCE(id_priv->state) != RDMA_CM_ADDR_BOUND &&
			    READ_ONCE(id_priv->state) != RDMA_CM_ADDR_RESOLVED))
		return -EINVAL;

	mc = kzalloc(sizeof(*mc), GFP_KERNEL);
	if (!mc)
		return -ENOMEM;

	memcpy(&mc->addr, addr, rdma_addr_size(addr));
	mc->context = context;
	mc->id_priv = id_priv;
	mc->join_state = join_state;

	if (rdma_protocol_roce(id->device, id->port_num)) {
		ret = cma_iboe_join_multicast(id_priv, mc);
		if (ret)
			goto out_err;
	} else if (rdma_cap_ib_mcast(id->device, id->port_num)) {
		ret = cma_join_ib_multicast(id_priv, mc);
		if (ret)
			goto out_err;
	} else {
		ret = -ENOSYS;
		goto out_err;
	}

	spin_lock(&id_priv->lock);
	list_add(&mc->list, &id_priv->mc_list);
	spin_unlock(&id_priv->lock);

	return 0;
out_err:
	kfree(mc);
	return ret;
}
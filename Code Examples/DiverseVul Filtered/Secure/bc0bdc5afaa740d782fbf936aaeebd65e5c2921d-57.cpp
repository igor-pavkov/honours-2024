int rdma_resolve_route(struct rdma_cm_id *id, unsigned long timeout_ms)
{
	struct rdma_id_private *id_priv;
	int ret;

	if (!timeout_ms)
		return -EINVAL;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (!cma_comp_exch(id_priv, RDMA_CM_ADDR_RESOLVED, RDMA_CM_ROUTE_QUERY))
		return -EINVAL;

	cma_id_get(id_priv);
	if (rdma_cap_ib_sa(id->device, id->port_num))
		ret = cma_resolve_ib_route(id_priv, timeout_ms);
	else if (rdma_protocol_roce(id->device, id->port_num))
		ret = cma_resolve_iboe_route(id_priv);
	else if (rdma_protocol_iwarp(id->device, id->port_num))
		ret = cma_resolve_iw_route(id_priv);
	else
		ret = -ENOSYS;

	if (ret)
		goto err;

	return 0;
err:
	cma_comp_exch(id_priv, RDMA_CM_ROUTE_QUERY, RDMA_CM_ADDR_RESOLVED);
	cma_id_put(id_priv);
	return ret;
}
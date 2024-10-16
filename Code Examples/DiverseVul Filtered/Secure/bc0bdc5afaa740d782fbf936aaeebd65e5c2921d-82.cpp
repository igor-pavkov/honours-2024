int rdma_connect_locked(struct rdma_cm_id *id,
			struct rdma_conn_param *conn_param)
{
	struct rdma_id_private *id_priv =
		container_of(id, struct rdma_id_private, id);
	int ret;

	if (!cma_comp_exch(id_priv, RDMA_CM_ROUTE_RESOLVED, RDMA_CM_CONNECT))
		return -EINVAL;

	if (!id->qp) {
		id_priv->qp_num = conn_param->qp_num;
		id_priv->srq = conn_param->srq;
	}

	if (rdma_cap_ib_cm(id->device, id->port_num)) {
		if (id->qp_type == IB_QPT_UD)
			ret = cma_resolve_ib_udp(id_priv, conn_param);
		else
			ret = cma_connect_ib(id_priv, conn_param);
	} else if (rdma_cap_iw_cm(id->device, id->port_num)) {
		ret = cma_connect_iw(id_priv, conn_param);
	} else {
		ret = -ENOSYS;
	}
	if (ret)
		goto err_state;
	return 0;
err_state:
	cma_comp_exch(id_priv, RDMA_CM_CONNECT, RDMA_CM_ROUTE_RESOLVED);
	return ret;
}
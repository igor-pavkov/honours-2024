int rdma_accept(struct rdma_cm_id *id, struct rdma_conn_param *conn_param)
{
	struct rdma_id_private *id_priv =
		container_of(id, struct rdma_id_private, id);
	int ret;

	lockdep_assert_held(&id_priv->handler_mutex);

	if (READ_ONCE(id_priv->state) != RDMA_CM_CONNECT)
		return -EINVAL;

	if (!id->qp && conn_param) {
		id_priv->qp_num = conn_param->qp_num;
		id_priv->srq = conn_param->srq;
	}

	if (rdma_cap_ib_cm(id->device, id->port_num)) {
		if (id->qp_type == IB_QPT_UD) {
			if (conn_param)
				ret = cma_send_sidr_rep(id_priv, IB_SIDR_SUCCESS,
							conn_param->qkey,
							conn_param->private_data,
							conn_param->private_data_len);
			else
				ret = cma_send_sidr_rep(id_priv, IB_SIDR_SUCCESS,
							0, NULL, 0);
		} else {
			if (conn_param)
				ret = cma_accept_ib(id_priv, conn_param);
			else
				ret = cma_rep_recv(id_priv);
		}
	} else if (rdma_cap_iw_cm(id->device, id->port_num)) {
		ret = cma_accept_iw(id_priv, conn_param);
	} else {
		ret = -ENOSYS;
	}
	if (ret)
		goto reject;

	return 0;
reject:
	cma_modify_qp_err(id_priv);
	rdma_reject(id, NULL, 0, IB_CM_REJ_CONSUMER_DEFINED);
	return ret;
}
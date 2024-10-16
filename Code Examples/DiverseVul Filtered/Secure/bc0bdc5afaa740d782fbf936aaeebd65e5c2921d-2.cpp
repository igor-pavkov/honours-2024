int rdma_init_qp_attr(struct rdma_cm_id *id, struct ib_qp_attr *qp_attr,
		       int *qp_attr_mask)
{
	struct rdma_id_private *id_priv;
	int ret = 0;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (rdma_cap_ib_cm(id->device, id->port_num)) {
		if (!id_priv->cm_id.ib || (id_priv->id.qp_type == IB_QPT_UD))
			ret = cma_ib_init_qp_attr(id_priv, qp_attr, qp_attr_mask);
		else
			ret = ib_cm_init_qp_attr(id_priv->cm_id.ib, qp_attr,
						 qp_attr_mask);

		if (qp_attr->qp_state == IB_QPS_RTR)
			qp_attr->rq_psn = id_priv->seq_num;
	} else if (rdma_cap_iw_cm(id->device, id->port_num)) {
		if (!id_priv->cm_id.iw) {
			qp_attr->qp_access_flags = 0;
			*qp_attr_mask = IB_QP_STATE | IB_QP_ACCESS_FLAGS;
		} else
			ret = iw_cm_init_qp_attr(id_priv->cm_id.iw, qp_attr,
						 qp_attr_mask);
		qp_attr->port_num = id_priv->id.port_num;
		*qp_attr_mask |= IB_QP_PORT;
	} else {
		ret = -ENOSYS;
	}

	if ((*qp_attr_mask & IB_QP_TIMEOUT) && id_priv->timeout_set)
		qp_attr->timeout = id_priv->timeout;

	if ((*qp_attr_mask & IB_QP_MIN_RNR_TIMER) && id_priv->min_rnr_timer_set)
		qp_attr->min_rnr_timer = id_priv->min_rnr_timer;

	return ret;
}
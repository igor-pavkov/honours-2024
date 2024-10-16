int rdma_create_qp(struct rdma_cm_id *id, struct ib_pd *pd,
		   struct ib_qp_init_attr *qp_init_attr)
{
	struct rdma_id_private *id_priv;
	struct ib_qp *qp;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (id->device != pd->device) {
		ret = -EINVAL;
		goto out_err;
	}

	qp_init_attr->port_num = id->port_num;
	qp = ib_create_qp(pd, qp_init_attr);
	if (IS_ERR(qp)) {
		ret = PTR_ERR(qp);
		goto out_err;
	}

	if (id->qp_type == IB_QPT_UD)
		ret = cma_init_ud_qp(id_priv, qp);
	else
		ret = cma_init_conn_qp(id_priv, qp);
	if (ret)
		goto out_destroy;

	id->qp = qp;
	id_priv->qp_num = qp->qp_num;
	id_priv->srq = (qp->srq != NULL);
	trace_cm_qp_create(id_priv, pd, qp_init_attr, 0);
	return 0;
out_destroy:
	ib_destroy_qp(qp);
out_err:
	trace_cm_qp_create(id_priv, pd, qp_init_attr, ret);
	return ret;
}
int rdma_set_ack_timeout(struct rdma_cm_id *id, u8 timeout)
{
	struct rdma_id_private *id_priv;

	if (id->qp_type != IB_QPT_RC)
		return -EINVAL;

	id_priv = container_of(id, struct rdma_id_private, id);
	mutex_lock(&id_priv->qp_mutex);
	id_priv->timeout = timeout;
	id_priv->timeout_set = true;
	mutex_unlock(&id_priv->qp_mutex);

	return 0;
}
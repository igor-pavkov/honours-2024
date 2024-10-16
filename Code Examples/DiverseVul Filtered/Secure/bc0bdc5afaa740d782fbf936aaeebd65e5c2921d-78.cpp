int rdma_set_min_rnr_timer(struct rdma_cm_id *id, u8 min_rnr_timer)
{
	struct rdma_id_private *id_priv;

	/* It is a five-bit value */
	if (min_rnr_timer & 0xe0)
		return -EINVAL;

	if (WARN_ON(id->qp_type != IB_QPT_RC && id->qp_type != IB_QPT_XRC_TGT))
		return -EINVAL;

	id_priv = container_of(id, struct rdma_id_private, id);
	mutex_lock(&id_priv->qp_mutex);
	id_priv->min_rnr_timer = min_rnr_timer;
	id_priv->min_rnr_timer_set = true;
	mutex_unlock(&id_priv->qp_mutex);

	return 0;
}
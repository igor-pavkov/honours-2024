void rdma_lock_handler(struct rdma_cm_id *id)
{
	struct rdma_id_private *id_priv =
		container_of(id, struct rdma_id_private, id);

	mutex_lock(&id_priv->handler_mutex);
}
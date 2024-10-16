void rdma_unlock_handler(struct rdma_cm_id *id)
{
	struct rdma_id_private *id_priv =
		container_of(id, struct rdma_id_private, id);

	mutex_unlock(&id_priv->handler_mutex);
}
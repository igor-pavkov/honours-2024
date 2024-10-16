void rdma_destroy_id(struct rdma_cm_id *id)
{
	struct rdma_id_private *id_priv =
		container_of(id, struct rdma_id_private, id);

	mutex_lock(&id_priv->handler_mutex);
	destroy_id_handler_unlock(id_priv);
}
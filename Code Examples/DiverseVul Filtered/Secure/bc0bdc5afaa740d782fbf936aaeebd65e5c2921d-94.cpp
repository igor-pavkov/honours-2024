int rdma_connect(struct rdma_cm_id *id, struct rdma_conn_param *conn_param)
{
	struct rdma_id_private *id_priv =
		container_of(id, struct rdma_id_private, id);
	int ret;

	mutex_lock(&id_priv->handler_mutex);
	ret = rdma_connect_locked(id, conn_param);
	mutex_unlock(&id_priv->handler_mutex);
	return ret;
}
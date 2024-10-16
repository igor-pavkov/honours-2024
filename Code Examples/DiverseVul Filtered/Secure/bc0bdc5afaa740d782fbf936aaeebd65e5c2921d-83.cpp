struct iw_cm_id *rdma_iw_cm_id(struct rdma_cm_id *id)
{
	struct rdma_id_private *id_priv;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (id->device->node_type == RDMA_NODE_RNIC)
		return id_priv->cm_id.iw;
	return NULL;
}
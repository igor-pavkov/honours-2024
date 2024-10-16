struct rdma_cm_id *rdma_res_to_id(struct rdma_restrack_entry *res)
{
	struct rdma_id_private *id_priv =
		container_of(res, struct rdma_id_private, res);

	return &id_priv->id;
}
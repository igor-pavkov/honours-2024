int rdma_connect_ece(struct rdma_cm_id *id, struct rdma_conn_param *conn_param,
		     struct rdma_ucm_ece *ece)
{
	struct rdma_id_private *id_priv =
		container_of(id, struct rdma_id_private, id);

	id_priv->ece.vendor_id = ece->vendor_id;
	id_priv->ece.attr_mod = ece->attr_mod;

	return rdma_connect(id, conn_param);
}
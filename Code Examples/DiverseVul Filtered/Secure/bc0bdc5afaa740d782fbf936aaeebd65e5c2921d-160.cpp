static int cma_ib_listen(struct rdma_id_private *id_priv)
{
	struct sockaddr *addr;
	struct ib_cm_id	*id;
	__be64 svc_id;

	addr = cma_src_addr(id_priv);
	svc_id = rdma_get_service_id(&id_priv->id, addr);
	id = ib_cm_insert_listen(id_priv->id.device,
				 cma_ib_req_handler, svc_id);
	if (IS_ERR(id))
		return PTR_ERR(id);
	id_priv->cm_id.ib = id;

	return 0;
}
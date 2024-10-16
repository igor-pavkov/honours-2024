static void cma_bind_sgid_attr(struct rdma_id_private *id_priv,
			       const struct ib_gid_attr *sgid_attr)
{
	WARN_ON(id_priv->id.route.addr.dev_addr.sgid_attr);
	id_priv->id.route.addr.dev_addr.sgid_attr = sgid_attr;
}
static void cma_id_put(struct rdma_id_private *id_priv)
{
	if (refcount_dec_and_test(&id_priv->refcount))
		complete(&id_priv->comp);
}
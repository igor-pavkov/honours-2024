static void cma_release_dev(struct rdma_id_private *id_priv)
{
	mutex_lock(&lock);
	list_del(&id_priv->list);
	cma_dev_put(id_priv->cma_dev);
	id_priv->cma_dev = NULL;
	id_priv->id.device = NULL;
	if (id_priv->id.route.addr.dev_addr.sgid_attr) {
		rdma_put_gid_attr(id_priv->id.route.addr.dev_addr.sgid_attr);
		id_priv->id.route.addr.dev_addr.sgid_attr = NULL;
	}
	mutex_unlock(&lock);
}
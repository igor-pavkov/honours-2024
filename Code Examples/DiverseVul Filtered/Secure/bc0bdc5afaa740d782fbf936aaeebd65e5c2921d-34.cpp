static void cma_cancel_listens(struct rdma_id_private *id_priv)
{
	mutex_lock(&lock);
	_cma_cancel_listens(id_priv);
	mutex_unlock(&lock);
}
static void _cma_cancel_listens(struct rdma_id_private *id_priv)
{
	struct rdma_id_private *dev_id_priv;

	lockdep_assert_held(&lock);

	/*
	 * Remove from listen_any_list to prevent added devices from spawning
	 * additional listen requests.
	 */
	list_del(&id_priv->list);

	while (!list_empty(&id_priv->listen_list)) {
		dev_id_priv = list_entry(id_priv->listen_list.next,
					 struct rdma_id_private, listen_list);
		/* sync with device removal to avoid duplicate destruction */
		list_del_init(&dev_id_priv->list);
		list_del(&dev_id_priv->listen_list);
		mutex_unlock(&lock);

		rdma_destroy_id(&dev_id_priv->id);
		mutex_lock(&lock);
	}
}
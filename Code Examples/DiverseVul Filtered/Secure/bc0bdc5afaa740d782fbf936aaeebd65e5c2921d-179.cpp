static void cma_process_remove(struct cma_device *cma_dev)
{
	mutex_lock(&lock);
	while (!list_empty(&cma_dev->id_list)) {
		struct rdma_id_private *id_priv = list_first_entry(
			&cma_dev->id_list, struct rdma_id_private, list);

		list_del(&id_priv->listen_list);
		list_del_init(&id_priv->list);
		cma_id_get(id_priv);
		mutex_unlock(&lock);

		cma_send_device_removal_put(id_priv);

		mutex_lock(&lock);
	}
	mutex_unlock(&lock);

	cma_dev_put(cma_dev);
	wait_for_completion(&cma_dev->comp);
}
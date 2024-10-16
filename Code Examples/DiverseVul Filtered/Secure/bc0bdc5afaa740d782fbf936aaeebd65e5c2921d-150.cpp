static int cma_listen_on_all(struct rdma_id_private *id_priv)
{
	struct rdma_id_private *to_destroy;
	struct cma_device *cma_dev;
	int ret;

	mutex_lock(&lock);
	list_add_tail(&id_priv->list, &listen_any_list);
	list_for_each_entry(cma_dev, &dev_list, list) {
		ret = cma_listen_on_dev(id_priv, cma_dev, &to_destroy);
		if (ret) {
			/* Prevent racing with cma_process_remove() */
			if (to_destroy)
				list_del_init(&to_destroy->list);
			goto err_listen;
		}
	}
	mutex_unlock(&lock);
	return 0;

err_listen:
	_cma_cancel_listens(id_priv);
	mutex_unlock(&lock);
	if (to_destroy)
		rdma_destroy_id(&to_destroy->id);
	return ret;
}
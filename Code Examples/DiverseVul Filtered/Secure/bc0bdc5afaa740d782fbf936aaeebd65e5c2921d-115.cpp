static struct rdma_id_private *cma_find_listener(
		const struct rdma_bind_list *bind_list,
		const struct ib_cm_id *cm_id,
		const struct ib_cm_event *ib_event,
		const struct cma_req_info *req,
		const struct net_device *net_dev)
{
	struct rdma_id_private *id_priv, *id_priv_dev;

	lockdep_assert_held(&lock);

	if (!bind_list)
		return ERR_PTR(-EINVAL);

	hlist_for_each_entry(id_priv, &bind_list->owners, node) {
		if (cma_match_private_data(id_priv, ib_event->private_data)) {
			if (id_priv->id.device == cm_id->device &&
			    cma_match_net_dev(&id_priv->id, net_dev, req))
				return id_priv;
			list_for_each_entry(id_priv_dev,
					    &id_priv->listen_list,
					    listen_list) {
				if (id_priv_dev->id.device == cm_id->device &&
				    cma_match_net_dev(&id_priv_dev->id,
						      net_dev, req))
					return id_priv_dev;
			}
		}
	}

	return ERR_PTR(-EINVAL);
}
static int cma_add_one(struct ib_device *device)
{
	struct rdma_id_private *to_destroy;
	struct cma_device *cma_dev;
	struct rdma_id_private *id_priv;
	unsigned long supported_gids = 0;
	int ret;
	u32 i;

	if (!cma_supported(device))
		return -EOPNOTSUPP;

	cma_dev = kmalloc(sizeof(*cma_dev), GFP_KERNEL);
	if (!cma_dev)
		return -ENOMEM;

	cma_dev->device = device;
	cma_dev->default_gid_type = kcalloc(device->phys_port_cnt,
					    sizeof(*cma_dev->default_gid_type),
					    GFP_KERNEL);
	if (!cma_dev->default_gid_type) {
		ret = -ENOMEM;
		goto free_cma_dev;
	}

	cma_dev->default_roce_tos = kcalloc(device->phys_port_cnt,
					    sizeof(*cma_dev->default_roce_tos),
					    GFP_KERNEL);
	if (!cma_dev->default_roce_tos) {
		ret = -ENOMEM;
		goto free_gid_type;
	}

	rdma_for_each_port (device, i) {
		supported_gids = roce_gid_type_mask_support(device, i);
		WARN_ON(!supported_gids);
		if (supported_gids & (1 << CMA_PREFERRED_ROCE_GID_TYPE))
			cma_dev->default_gid_type[i - rdma_start_port(device)] =
				CMA_PREFERRED_ROCE_GID_TYPE;
		else
			cma_dev->default_gid_type[i - rdma_start_port(device)] =
				find_first_bit(&supported_gids, BITS_PER_LONG);
		cma_dev->default_roce_tos[i - rdma_start_port(device)] = 0;
	}

	init_completion(&cma_dev->comp);
	refcount_set(&cma_dev->refcount, 1);
	INIT_LIST_HEAD(&cma_dev->id_list);
	ib_set_client_data(device, &cma_client, cma_dev);

	mutex_lock(&lock);
	list_add_tail(&cma_dev->list, &dev_list);
	list_for_each_entry(id_priv, &listen_any_list, list) {
		ret = cma_listen_on_dev(id_priv, cma_dev, &to_destroy);
		if (ret)
			goto free_listen;
	}
	mutex_unlock(&lock);

	trace_cm_add_one(device);
	return 0;

free_listen:
	list_del(&cma_dev->list);
	mutex_unlock(&lock);

	/* cma_process_remove() will delete to_destroy */
	cma_process_remove(cma_dev);
	kfree(cma_dev->default_roce_tos);
free_gid_type:
	kfree(cma_dev->default_gid_type);

free_cma_dev:
	kfree(cma_dev);
	return ret;
}
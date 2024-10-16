int rdma_set_ib_path(struct rdma_cm_id *id,
		     struct sa_path_rec *path_rec)
{
	struct rdma_id_private *id_priv;
	struct net_device *ndev;
	int ret;

	id_priv = container_of(id, struct rdma_id_private, id);
	if (!cma_comp_exch(id_priv, RDMA_CM_ADDR_RESOLVED,
			   RDMA_CM_ROUTE_RESOLVED))
		return -EINVAL;

	id->route.path_rec = kmemdup(path_rec, sizeof(*path_rec),
				     GFP_KERNEL);
	if (!id->route.path_rec) {
		ret = -ENOMEM;
		goto err;
	}

	if (rdma_protocol_roce(id->device, id->port_num)) {
		ndev = cma_iboe_set_path_rec_l2_fields(id_priv);
		if (!ndev) {
			ret = -ENODEV;
			goto err_free;
		}
		dev_put(ndev);
	}

	id->route.num_paths = 1;
	return 0;

err_free:
	kfree(id->route.path_rec);
	id->route.path_rec = NULL;
err:
	cma_comp_exch(id_priv, RDMA_CM_ROUTE_RESOLVED, RDMA_CM_ADDR_RESOLVED);
	return ret;
}
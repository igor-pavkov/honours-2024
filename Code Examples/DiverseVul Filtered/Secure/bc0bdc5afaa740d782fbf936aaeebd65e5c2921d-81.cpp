static int cma_iw_acquire_dev(struct rdma_id_private *id_priv,
			      const struct rdma_id_private *listen_id_priv)
{
	struct rdma_dev_addr *dev_addr = &id_priv->id.route.addr.dev_addr;
	const struct ib_gid_attr *sgid_attr;
	struct cma_device *cma_dev;
	enum ib_gid_type gid_type;
	int ret = -ENODEV;
	union ib_gid gid;
	u32 port;

	if (dev_addr->dev_type != ARPHRD_INFINIBAND &&
	    id_priv->id.ps == RDMA_PS_IPOIB)
		return -EINVAL;

	memcpy(&gid, dev_addr->src_dev_addr +
	       rdma_addr_gid_offset(dev_addr), sizeof(gid));

	mutex_lock(&lock);

	cma_dev = listen_id_priv->cma_dev;
	port = listen_id_priv->id.port_num;
	gid_type = listen_id_priv->gid_type;
	sgid_attr = cma_validate_port(cma_dev->device, port,
				      gid_type, &gid, id_priv);
	if (!IS_ERR(sgid_attr)) {
		id_priv->id.port_num = port;
		cma_bind_sgid_attr(id_priv, sgid_attr);
		ret = 0;
		goto out;
	}

	list_for_each_entry(cma_dev, &dev_list, list) {
		rdma_for_each_port (cma_dev->device, port) {
			if (listen_id_priv->cma_dev == cma_dev &&
			    listen_id_priv->id.port_num == port)
				continue;

			gid_type = cma_dev->default_gid_type[port - 1];
			sgid_attr = cma_validate_port(cma_dev->device, port,
						      gid_type, &gid, id_priv);
			if (!IS_ERR(sgid_attr)) {
				id_priv->id.port_num = port;
				cma_bind_sgid_attr(id_priv, sgid_attr);
				ret = 0;
				goto out;
			}
		}
	}

out:
	if (!ret) {
		cma_attach_to_dev(id_priv, cma_dev);
		rdma_restrack_add(&id_priv->res);
	}

	mutex_unlock(&lock);
	return ret;
}
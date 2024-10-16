static int cma_acquire_dev_by_src_ip(struct rdma_id_private *id_priv)
{
	struct rdma_dev_addr *dev_addr = &id_priv->id.route.addr.dev_addr;
	const struct ib_gid_attr *sgid_attr;
	union ib_gid gid, iboe_gid, *gidp;
	struct cma_device *cma_dev;
	enum ib_gid_type gid_type;
	int ret = -ENODEV;
	u32 port;

	if (dev_addr->dev_type != ARPHRD_INFINIBAND &&
	    id_priv->id.ps == RDMA_PS_IPOIB)
		return -EINVAL;

	rdma_ip2gid((struct sockaddr *)&id_priv->id.route.addr.src_addr,
		    &iboe_gid);

	memcpy(&gid, dev_addr->src_dev_addr +
	       rdma_addr_gid_offset(dev_addr), sizeof(gid));

	mutex_lock(&lock);
	list_for_each_entry(cma_dev, &dev_list, list) {
		rdma_for_each_port (cma_dev->device, port) {
			gidp = rdma_protocol_roce(cma_dev->device, port) ?
			       &iboe_gid : &gid;
			gid_type = cma_dev->default_gid_type[port - 1];
			sgid_attr = cma_validate_port(cma_dev->device, port,
						      gid_type, gidp, id_priv);
			if (!IS_ERR(sgid_attr)) {
				id_priv->id.port_num = port;
				cma_bind_sgid_attr(id_priv, sgid_attr);
				cma_attach_to_dev(id_priv, cma_dev);
				ret = 0;
				goto out;
			}
		}
	}
out:
	mutex_unlock(&lock);
	return ret;
}
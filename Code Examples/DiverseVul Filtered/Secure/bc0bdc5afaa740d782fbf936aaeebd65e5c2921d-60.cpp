static int cma_bind_loopback(struct rdma_id_private *id_priv)
{
	struct cma_device *cma_dev, *cur_dev;
	union ib_gid gid;
	enum ib_port_state port_state;
	unsigned int p;
	u16 pkey;
	int ret;

	cma_dev = NULL;
	mutex_lock(&lock);
	list_for_each_entry(cur_dev, &dev_list, list) {
		if (cma_family(id_priv) == AF_IB &&
		    !rdma_cap_ib_cm(cur_dev->device, 1))
			continue;

		if (!cma_dev)
			cma_dev = cur_dev;

		rdma_for_each_port (cur_dev->device, p) {
			if (!ib_get_cached_port_state(cur_dev->device, p, &port_state) &&
			    port_state == IB_PORT_ACTIVE) {
				cma_dev = cur_dev;
				goto port_found;
			}
		}
	}

	if (!cma_dev) {
		ret = -ENODEV;
		goto out;
	}

	p = 1;

port_found:
	ret = rdma_query_gid(cma_dev->device, p, 0, &gid);
	if (ret)
		goto out;

	ret = ib_get_cached_pkey(cma_dev->device, p, 0, &pkey);
	if (ret)
		goto out;

	id_priv->id.route.addr.dev_addr.dev_type =
		(rdma_protocol_ib(cma_dev->device, p)) ?
		ARPHRD_INFINIBAND : ARPHRD_ETHER;

	rdma_addr_set_sgid(&id_priv->id.route.addr.dev_addr, &gid);
	ib_addr_set_pkey(&id_priv->id.route.addr.dev_addr, pkey);
	id_priv->id.port_num = p;
	cma_attach_to_dev(id_priv, cma_dev);
	rdma_restrack_add(&id_priv->res);
	cma_set_loopback(cma_src_addr(id_priv));
out:
	mutex_unlock(&lock);
	return ret;
}
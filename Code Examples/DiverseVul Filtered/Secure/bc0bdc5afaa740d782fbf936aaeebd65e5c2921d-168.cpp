static int cma_resolve_ib_dev(struct rdma_id_private *id_priv)
{
	struct cma_device *cma_dev, *cur_dev;
	struct sockaddr_ib *addr;
	union ib_gid gid, sgid, *dgid;
	unsigned int p;
	u16 pkey, index;
	enum ib_port_state port_state;
	int i;

	cma_dev = NULL;
	addr = (struct sockaddr_ib *) cma_dst_addr(id_priv);
	dgid = (union ib_gid *) &addr->sib_addr;
	pkey = ntohs(addr->sib_pkey);

	mutex_lock(&lock);
	list_for_each_entry(cur_dev, &dev_list, list) {
		rdma_for_each_port (cur_dev->device, p) {
			if (!rdma_cap_af_ib(cur_dev->device, p))
				continue;

			if (ib_find_cached_pkey(cur_dev->device, p, pkey, &index))
				continue;

			if (ib_get_cached_port_state(cur_dev->device, p, &port_state))
				continue;
			for (i = 0; !rdma_query_gid(cur_dev->device,
						    p, i, &gid);
			     i++) {
				if (!memcmp(&gid, dgid, sizeof(gid))) {
					cma_dev = cur_dev;
					sgid = gid;
					id_priv->id.port_num = p;
					goto found;
				}

				if (!cma_dev && (gid.global.subnet_prefix ==
				    dgid->global.subnet_prefix) &&
				    port_state == IB_PORT_ACTIVE) {
					cma_dev = cur_dev;
					sgid = gid;
					id_priv->id.port_num = p;
					goto found;
				}
			}
		}
	}
	mutex_unlock(&lock);
	return -ENODEV;

found:
	cma_attach_to_dev(id_priv, cma_dev);
	rdma_restrack_add(&id_priv->res);
	mutex_unlock(&lock);
	addr = (struct sockaddr_ib *)cma_src_addr(id_priv);
	memcpy(&addr->sib_addr, &sgid, sizeof(sgid));
	cma_translate_ib(addr, &id_priv->id.route.addr.dev_addr);
	return 0;
}
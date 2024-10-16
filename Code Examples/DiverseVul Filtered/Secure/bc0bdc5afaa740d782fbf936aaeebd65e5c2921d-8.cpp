static int cma_ib_acquire_dev(struct rdma_id_private *id_priv,
			      const struct rdma_id_private *listen_id_priv,
			      struct cma_req_info *req)
{
	struct rdma_dev_addr *dev_addr = &id_priv->id.route.addr.dev_addr;
	const struct ib_gid_attr *sgid_attr;
	enum ib_gid_type gid_type;
	union ib_gid gid;

	if (dev_addr->dev_type != ARPHRD_INFINIBAND &&
	    id_priv->id.ps == RDMA_PS_IPOIB)
		return -EINVAL;

	if (rdma_protocol_roce(req->device, req->port))
		rdma_ip2gid((struct sockaddr *)&id_priv->id.route.addr.src_addr,
			    &gid);
	else
		memcpy(&gid, dev_addr->src_dev_addr +
		       rdma_addr_gid_offset(dev_addr), sizeof(gid));

	gid_type = listen_id_priv->cma_dev->default_gid_type[req->port - 1];
	sgid_attr = cma_validate_port(req->device, req->port,
				      gid_type, &gid, id_priv);
	if (IS_ERR(sgid_attr))
		return PTR_ERR(sgid_attr);

	id_priv->id.port_num = req->port;
	cma_bind_sgid_attr(id_priv, sgid_attr);
	/* Need to acquire lock to protect against reader
	 * of cma_dev->id_list such as cma_netdev_callback() and
	 * cma_process_remove().
	 */
	mutex_lock(&lock);
	cma_attach_to_dev(id_priv, listen_id_priv->cma_dev);
	mutex_unlock(&lock);
	rdma_restrack_add(&id_priv->res);
	return 0;
}
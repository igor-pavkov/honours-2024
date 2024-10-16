static int cma_resolve_ib_addr(struct rdma_id_private *id_priv)
{
	struct cma_work *work;
	int ret;

	work = kzalloc(sizeof *work, GFP_KERNEL);
	if (!work)
		return -ENOMEM;

	if (!id_priv->cma_dev) {
		ret = cma_resolve_ib_dev(id_priv);
		if (ret)
			goto err;
	}

	rdma_addr_set_dgid(&id_priv->id.route.addr.dev_addr, (union ib_gid *)
		&(((struct sockaddr_ib *) &id_priv->id.route.addr.dst_addr)->sib_addr));

	enqueue_resolve_addr_work(work, id_priv);
	return 0;
err:
	kfree(work);
	return ret;
}
static int cma_listen_on_dev(struct rdma_id_private *id_priv,
			     struct cma_device *cma_dev,
			     struct rdma_id_private **to_destroy)
{
	struct rdma_id_private *dev_id_priv;
	struct net *net = id_priv->id.route.addr.dev_addr.net;
	int ret;

	lockdep_assert_held(&lock);

	*to_destroy = NULL;
	if (cma_family(id_priv) == AF_IB && !rdma_cap_ib_cm(cma_dev->device, 1))
		return 0;

	dev_id_priv =
		__rdma_create_id(net, cma_listen_handler, id_priv,
				 id_priv->id.ps, id_priv->id.qp_type, id_priv);
	if (IS_ERR(dev_id_priv))
		return PTR_ERR(dev_id_priv);

	dev_id_priv->state = RDMA_CM_ADDR_BOUND;
	memcpy(cma_src_addr(dev_id_priv), cma_src_addr(id_priv),
	       rdma_addr_size(cma_src_addr(id_priv)));

	_cma_attach_to_dev(dev_id_priv, cma_dev);
	rdma_restrack_add(&dev_id_priv->res);
	cma_id_get(id_priv);
	dev_id_priv->internal_id = 1;
	dev_id_priv->afonly = id_priv->afonly;
	mutex_lock(&id_priv->qp_mutex);
	dev_id_priv->tos_set = id_priv->tos_set;
	dev_id_priv->tos = id_priv->tos;
	mutex_unlock(&id_priv->qp_mutex);

	ret = rdma_listen(&dev_id_priv->id, id_priv->backlog);
	if (ret)
		goto err_listen;
	list_add_tail(&dev_id_priv->listen_list, &id_priv->listen_list);
	return 0;
err_listen:
	/* Caller must destroy this after releasing lock */
	*to_destroy = dev_id_priv;
	dev_warn(&cma_dev->device->dev, "RDMA CMA: %s, error %d\n", __func__, ret);
	return ret;
}
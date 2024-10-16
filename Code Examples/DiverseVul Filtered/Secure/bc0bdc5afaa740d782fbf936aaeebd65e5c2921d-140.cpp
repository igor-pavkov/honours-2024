static int cma_alloc_port(enum rdma_ucm_port_space ps,
			  struct rdma_id_private *id_priv, unsigned short snum)
{
	struct rdma_bind_list *bind_list;
	int ret;

	lockdep_assert_held(&lock);

	bind_list = kzalloc(sizeof *bind_list, GFP_KERNEL);
	if (!bind_list)
		return -ENOMEM;

	ret = cma_ps_alloc(id_priv->id.route.addr.dev_addr.net, ps, bind_list,
			   snum);
	if (ret < 0)
		goto err;

	bind_list->ps = ps;
	bind_list->port = snum;
	cma_bind_port(bind_list, id_priv);
	return 0;
err:
	kfree(bind_list);
	return ret == -ENOSPC ? -EADDRNOTAVAIL : ret;
}
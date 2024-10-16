static int cma_alloc_any_port(enum rdma_ucm_port_space ps,
			      struct rdma_id_private *id_priv)
{
	static unsigned int last_used_port;
	int low, high, remaining;
	unsigned int rover;
	struct net *net = id_priv->id.route.addr.dev_addr.net;

	lockdep_assert_held(&lock);

	inet_get_local_port_range(net, &low, &high);
	remaining = (high - low) + 1;
	rover = prandom_u32() % remaining + low;
retry:
	if (last_used_port != rover) {
		struct rdma_bind_list *bind_list;
		int ret;

		bind_list = cma_ps_find(net, ps, (unsigned short)rover);

		if (!bind_list) {
			ret = cma_alloc_port(ps, id_priv, rover);
		} else {
			ret = cma_port_is_unique(bind_list, id_priv);
			if (!ret)
				cma_bind_port(bind_list, id_priv);
		}
		/*
		 * Remember previously used port number in order to avoid
		 * re-using same port immediately after it is closed.
		 */
		if (!ret)
			last_used_port = rover;
		if (ret != -EADDRNOTAVAIL)
			return ret;
	}
	if (--remaining) {
		rover++;
		if ((rover < low) || (rover > high))
			rover = low;
		goto retry;
	}
	return -EADDRNOTAVAIL;
}
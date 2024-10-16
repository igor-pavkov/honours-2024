struct net *copy_net_ns(unsigned long flags,
			struct user_namespace *user_ns, struct net *old_net)
{
	struct ucounts *ucounts;
	struct net *net;
	int rv;

	if (!(flags & CLONE_NEWNET))
		return get_net(old_net);

	ucounts = inc_net_namespaces(user_ns);
	if (!ucounts)
		return ERR_PTR(-ENOSPC);

	net = net_alloc();
	if (!net) {
		dec_net_namespaces(ucounts);
		return ERR_PTR(-ENOMEM);
	}

	get_user_ns(user_ns);

	rv = mutex_lock_killable(&net_mutex);
	if (rv < 0) {
		net_free(net);
		dec_net_namespaces(ucounts);
		put_user_ns(user_ns);
		return ERR_PTR(rv);
	}

	net->ucounts = ucounts;
	rv = setup_net(net, user_ns);
	if (rv == 0) {
		rtnl_lock();
		list_add_tail_rcu(&net->list, &net_namespace_list);
		rtnl_unlock();
	}
	mutex_unlock(&net_mutex);
	if (rv < 0) {
		dec_net_namespaces(ucounts);
		put_user_ns(user_ns);
		net_drop_ns(net);
		return ERR_PTR(rv);
	}
	return net;
}
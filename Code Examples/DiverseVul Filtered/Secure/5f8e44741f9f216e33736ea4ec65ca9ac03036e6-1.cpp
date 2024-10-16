static int rtnl_group_dellink(const struct net *net, int group)
{
	struct net_device *dev, *aux;
	LIST_HEAD(list_kill);
	bool found = false;

	if (!group)
		return -EPERM;

	for_each_netdev(net, dev) {
		if (dev->group == group) {
			const struct rtnl_link_ops *ops;

			found = true;
			ops = dev->rtnl_link_ops;
			if (!ops || !ops->dellink)
				return -EOPNOTSUPP;
		}
	}

	if (!found)
		return -ENODEV;

	for_each_netdev_safe(net, dev, aux) {
		if (dev->group == group) {
			const struct rtnl_link_ops *ops;

			ops = dev->rtnl_link_ops;
			ops->dellink(dev, &list_kill);
		}
	}
	unregister_netdevice_many(&list_kill);

	return 0;
}
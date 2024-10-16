int rtnl_delete_link(struct net_device *dev)
{
	const struct rtnl_link_ops *ops;
	LIST_HEAD(list_kill);

	ops = dev->rtnl_link_ops;
	if (!ops || !ops->dellink)
		return -EOPNOTSUPP;

	ops->dellink(dev, &list_kill);
	unregister_netdevice_many(&list_kill);

	return 0;
}
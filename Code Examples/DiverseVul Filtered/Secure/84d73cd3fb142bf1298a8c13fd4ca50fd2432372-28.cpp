int __rtnl_link_register(struct rtnl_link_ops *ops)
{
	if (rtnl_link_ops_get(ops->kind))
		return -EEXIST;

	if (!ops->dellink)
		ops->dellink = unregister_netdevice_queue;

	list_add_tail(&ops->list, &link_ops);
	return 0;
}
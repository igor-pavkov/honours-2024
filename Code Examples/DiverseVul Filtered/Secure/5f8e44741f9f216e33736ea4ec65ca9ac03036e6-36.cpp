int __rtnl_link_register(struct rtnl_link_ops *ops)
{
	if (rtnl_link_ops_get(ops->kind))
		return -EEXIST;

	/* The check for setup is here because if ops
	 * does not have that filled up, it is not possible
	 * to use the ops for creating device. So do not
	 * fill up dellink as well. That disables rtnl_dellink.
	 */
	if (ops->setup && !ops->dellink)
		ops->dellink = unregister_netdevice_queue;

	list_add_tail(&ops->list, &link_ops);
	return 0;
}
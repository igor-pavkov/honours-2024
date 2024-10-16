void rtnl_link_unregister(struct rtnl_link_ops *ops)
{
	/* Close the race with cleanup_net() */
	mutex_lock(&net_mutex);
	rtnl_lock_unregistering_all();
	__rtnl_link_unregister(ops);
	rtnl_unlock();
	mutex_unlock(&net_mutex);
}
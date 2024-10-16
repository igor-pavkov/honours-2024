int dev_get_alias(const struct net_device *dev, char *name, size_t len)
{
	const struct dev_ifalias *alias;
	int ret = 0;

	rcu_read_lock();
	alias = rcu_dereference(dev->ifalias);
	if (alias)
		ret = snprintf(name, len, "%s", alias->ifalias);
	rcu_read_unlock();

	return ret;
}
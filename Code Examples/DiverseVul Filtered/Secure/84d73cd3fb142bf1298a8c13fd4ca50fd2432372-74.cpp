static size_t rtnl_link_get_size(const struct net_device *dev)
{
	const struct rtnl_link_ops *ops = dev->rtnl_link_ops;
	size_t size;

	if (!ops)
		return 0;

	size = nla_total_size(sizeof(struct nlattr)) + /* IFLA_LINKINFO */
	       nla_total_size(strlen(ops->kind) + 1);  /* IFLA_INFO_KIND */

	if (ops->get_size)
		/* IFLA_INFO_DATA + nested data */
		size += nla_total_size(sizeof(struct nlattr)) +
			ops->get_size(dev);

	if (ops->get_xstats_size)
		/* IFLA_INFO_XSTATS */
		size += nla_total_size(ops->get_xstats_size(dev));

	return size;
}
static bool link_kind_filtered(const struct net_device *dev,
			       const struct rtnl_link_ops *kind_ops)
{
	if (kind_ops && dev->rtnl_link_ops != kind_ops)
		return true;

	return false;
}
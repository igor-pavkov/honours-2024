static bool link_dump_filtered(struct net_device *dev,
			       int master_idx,
			       const struct rtnl_link_ops *kind_ops)
{
	if (link_master_filtered(dev, master_idx) ||
	    link_kind_filtered(dev, kind_ops))
		return true;

	return false;
}
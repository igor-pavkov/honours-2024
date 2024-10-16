static bool link_master_filtered(struct net_device *dev, int master_idx)
{
	struct net_device *master;

	if (!master_idx)
		return false;

	master = netdev_master_upper_dev_get(dev);
	if (!master || master->ifindex != master_idx)
		return true;

	return false;
}
static int ethtool_set_rx_csum(struct net_device *dev, char __user *useraddr)
{
	struct ethtool_value edata;

	if (!dev->ethtool_ops->set_rx_csum)
		return -EOPNOTSUPP;

	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;

	if (!edata.data && dev->ethtool_ops->set_sg)
		dev->features &= ~NETIF_F_GRO;

	return dev->ethtool_ops->set_rx_csum(dev, edata.data);
}
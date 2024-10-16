static int ethtool_set_gro(struct net_device *dev, char __user *useraddr)
{
	struct ethtool_value edata;

	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;

	if (edata.data) {
		if (!dev->ethtool_ops->get_rx_csum ||
		    !dev->ethtool_ops->get_rx_csum(dev))
			return -EINVAL;
		dev->features |= NETIF_F_GRO;
	} else
		dev->features &= ~NETIF_F_GRO;

	return 0;
}
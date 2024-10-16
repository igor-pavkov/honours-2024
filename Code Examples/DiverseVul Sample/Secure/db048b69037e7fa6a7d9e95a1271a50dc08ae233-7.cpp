static int ethtool_set_gso(struct net_device *dev, char __user *useraddr)
{
	struct ethtool_value edata;

	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;
	if (edata.data)
		dev->features |= NETIF_F_GSO;
	else
		dev->features &= ~NETIF_F_GSO;
	return 0;
}
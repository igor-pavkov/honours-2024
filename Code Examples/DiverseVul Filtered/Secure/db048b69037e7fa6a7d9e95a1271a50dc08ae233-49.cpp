static int ethtool_get_gso(struct net_device *dev, char __user *useraddr)
{
	struct ethtool_value edata = { ETHTOOL_GGSO };

	edata.data = dev->features & NETIF_F_GSO;
	if (copy_to_user(useraddr, &edata, sizeof(edata)))
		return -EFAULT;
	return 0;
}
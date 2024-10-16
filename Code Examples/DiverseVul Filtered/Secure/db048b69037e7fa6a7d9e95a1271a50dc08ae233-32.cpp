static int ethtool_get_gro(struct net_device *dev, char __user *useraddr)
{
	struct ethtool_value edata = { ETHTOOL_GGRO };

	edata.data = dev->features & NETIF_F_GRO;
	if (copy_to_user(useraddr, &edata, sizeof(edata)))
		return -EFAULT;
	return 0;
}
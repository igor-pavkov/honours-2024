static int ethtool_set_ufo(struct net_device *dev, char __user *useraddr)
{
	struct ethtool_value edata;

	if (!dev->ethtool_ops->set_ufo)
		return -EOPNOTSUPP;
	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;
	if (edata.data && !(dev->features & NETIF_F_SG))
		return -EINVAL;
	if (edata.data && !(dev->features & NETIF_F_HW_CSUM))
		return -EINVAL;
	return dev->ethtool_ops->set_ufo(dev, edata.data);
}
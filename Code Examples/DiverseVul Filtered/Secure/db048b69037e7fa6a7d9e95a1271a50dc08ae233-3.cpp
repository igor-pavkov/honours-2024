static int ethtool_set_sg(struct net_device *dev, char __user *useraddr)
{
	struct ethtool_value edata;

	if (!dev->ethtool_ops->set_sg)
		return -EOPNOTSUPP;

	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;

	if (edata.data &&
	    !(dev->features & NETIF_F_ALL_CSUM))
		return -EINVAL;

	return __ethtool_set_sg(dev, edata.data);
}
static int ethtool_set_tx_csum(struct net_device *dev, char __user *useraddr)
{
	struct ethtool_value edata;
	int err;

	if (!dev->ethtool_ops->set_tx_csum)
		return -EOPNOTSUPP;

	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;

	if (!edata.data && dev->ethtool_ops->set_sg) {
		err = __ethtool_set_sg(dev, 0);
		if (err)
			return err;
	}

	return dev->ethtool_ops->set_tx_csum(dev, edata.data);
}
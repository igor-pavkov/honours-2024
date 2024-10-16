static noinline_for_stack int ethtool_set_rxnfc(struct net_device *dev,
						void __user *useraddr)
{
	struct ethtool_rxnfc cmd;

	if (!dev->ethtool_ops->set_rxnfc)
		return -EOPNOTSUPP;

	if (copy_from_user(&cmd, useraddr, sizeof(cmd)))
		return -EFAULT;

	return dev->ethtool_ops->set_rxnfc(dev, &cmd);
}
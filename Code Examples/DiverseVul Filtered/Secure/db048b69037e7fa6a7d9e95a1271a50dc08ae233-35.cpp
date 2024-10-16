static noinline_for_stack int ethtool_flash_device(struct net_device *dev,
						   char __user *useraddr)
{
	struct ethtool_flash efl;

	if (copy_from_user(&efl, useraddr, sizeof(efl)))
		return -EFAULT;

	if (!dev->ethtool_ops->flash_device)
		return -EOPNOTSUPP;

	return dev->ethtool_ops->flash_device(dev, &efl);
}
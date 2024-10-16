static int ieee802154_dev_ioctl(struct sock *sk, struct ifreq __user *arg,
				unsigned int cmd)
{
	struct ifreq ifr;
	int ret = -ENOIOCTLCMD;
	struct net_device *dev;

	if (copy_from_user(&ifr, arg, sizeof(struct ifreq)))
		return -EFAULT;

	ifr.ifr_name[IFNAMSIZ-1] = 0;

	dev_load(sock_net(sk), ifr.ifr_name);
	dev = dev_get_by_name(sock_net(sk), ifr.ifr_name);

	if (!dev)
		return -ENODEV;

	if (dev->type == ARPHRD_IEEE802154 && dev->netdev_ops->ndo_do_ioctl)
		ret = dev->netdev_ops->ndo_do_ioctl(dev, &ifr, cmd);

	if (!ret && copy_to_user(arg, &ifr, sizeof(struct ifreq)))
		ret = -EFAULT;
	dev_put(dev);

	return ret;
}
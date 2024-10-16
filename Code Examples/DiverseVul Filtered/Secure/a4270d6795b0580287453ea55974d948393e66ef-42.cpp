int dev_open(struct net_device *dev, struct netlink_ext_ack *extack)
{
	int ret;

	if (dev->flags & IFF_UP)
		return 0;

	ret = __dev_open(dev, extack);
	if (ret < 0)
		return ret;

	rtmsg_ifinfo(RTM_NEWLINK, dev, IFF_UP|IFF_RUNNING, GFP_KERNEL);
	call_netdevice_notifiers(NETDEV_UP, dev);

	return ret;
}
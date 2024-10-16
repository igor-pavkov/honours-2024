static int call_netdevice_notifiers_extack(unsigned long val,
					   struct net_device *dev,
					   struct netlink_ext_ack *extack)
{
	struct netdev_notifier_info info = {
		.dev = dev,
		.extack = extack,
	};

	return call_netdevice_notifiers_info(val, &info);
}
static int call_netdevice_notifiers_mtu(unsigned long val,
					struct net_device *dev, u32 arg)
{
	struct netdev_notifier_info_ext info = {
		.info.dev = dev,
		.ext.mtu = arg,
	};

	BUILD_BUG_ON(offsetof(struct netdev_notifier_info_ext, info) != 0);

	return call_netdevice_notifiers_info(val, &info.info);
}
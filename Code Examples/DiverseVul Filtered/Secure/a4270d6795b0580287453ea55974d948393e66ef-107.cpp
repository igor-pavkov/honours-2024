static int call_netdevice_notifier(struct notifier_block *nb, unsigned long val,
				   struct net_device *dev)
{
	struct netdev_notifier_info info = {
		.dev = dev,
	};

	return nb->notifier_call(nb, val, &info);
}
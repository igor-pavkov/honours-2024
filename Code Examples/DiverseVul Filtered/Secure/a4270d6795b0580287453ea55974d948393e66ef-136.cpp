void netdev_state_change(struct net_device *dev)
{
	if (dev->flags & IFF_UP) {
		struct netdev_notifier_change_info change_info = {
			.info.dev = dev,
		};

		call_netdevice_notifiers_info(NETDEV_CHANGE,
					      &change_info.info);
		rtmsg_ifinfo(RTM_NEWLINK, dev, 0, GFP_KERNEL);
	}
}
static int rtnetlink_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	switch (event) {
	case NETDEV_UP:
	case NETDEV_DOWN:
	case NETDEV_PRE_UP:
	case NETDEV_POST_INIT:
	case NETDEV_REGISTER:
	case NETDEV_CHANGE:
	case NETDEV_PRE_TYPE_CHANGE:
	case NETDEV_GOING_DOWN:
	case NETDEV_UNREGISTER:
	case NETDEV_UNREGISTER_FINAL:
	case NETDEV_RELEASE:
	case NETDEV_JOIN:
	case NETDEV_BONDING_INFO:
		break;
	default:
		rtmsg_ifinfo(RTM_NEWLINK, dev, 0, GFP_KERNEL);
		break;
	}
	return NOTIFY_DONE;
}
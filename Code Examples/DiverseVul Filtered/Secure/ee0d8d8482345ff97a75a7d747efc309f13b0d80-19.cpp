static int ipxitf_device_event(struct notifier_block *notifier,
				unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct ipx_interface *i, *tmp;

	if (!net_eq(dev_net(dev), &init_net))
		return NOTIFY_DONE;

	if (event != NETDEV_DOWN && event != NETDEV_UP)
		goto out;

	spin_lock_bh(&ipx_interfaces_lock);
	list_for_each_entry_safe(i, tmp, &ipx_interfaces, node)
		if (i->if_dev == dev) {
			if (event == NETDEV_UP)
				ipxitf_hold(i);
			else
				__ipxitf_put(i);
		}
	spin_unlock_bh(&ipx_interfaces_lock);
out:
	return NOTIFY_DONE;
}
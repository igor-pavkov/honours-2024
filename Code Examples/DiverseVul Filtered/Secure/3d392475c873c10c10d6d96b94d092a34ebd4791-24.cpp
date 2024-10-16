static int ddp_device_event(struct notifier_block *this, unsigned long event,
			    void *ptr)
{
	struct net_device *dev = ptr;

	if (!net_eq(dev_net(dev), &init_net))
		return NOTIFY_DONE;

	if (event == NETDEV_DOWN)
		/* Discard any use of this */
		atalk_dev_down(dev);

	return NOTIFY_DONE;
}
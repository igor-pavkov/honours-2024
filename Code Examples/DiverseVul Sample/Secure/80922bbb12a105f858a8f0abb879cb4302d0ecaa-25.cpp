static int econet_notifier(struct notifier_block *this, unsigned long msg, void *data)
{
	struct net_device *dev = (struct net_device *)data;
	struct ec_device *edev;

	if (!net_eq(dev_net(dev), &init_net))
		return NOTIFY_DONE;

	switch (msg) {
	case NETDEV_UNREGISTER:
		/* A device has gone down - kill any data we hold for it. */
		edev = dev->ec_ptr;
		if (edev)
		{
			if (net2dev_map[0] == dev)
				net2dev_map[0] = NULL;
			net2dev_map[edev->net] = NULL;
			kfree(edev);
			dev->ec_ptr = NULL;
		}
		break;
	}

	return NOTIFY_DONE;
}
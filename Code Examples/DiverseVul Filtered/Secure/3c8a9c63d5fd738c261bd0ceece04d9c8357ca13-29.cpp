static int set_offload(struct net_device *dev, unsigned long arg)
{
	unsigned int old_features, features;

	old_features = dev->features;
	/* Unset features, set them as we chew on the arg. */
	features = (old_features & ~(NETIF_F_HW_CSUM|NETIF_F_SG|NETIF_F_FRAGLIST
				    |NETIF_F_TSO_ECN|NETIF_F_TSO|NETIF_F_TSO6));

	if (arg & TUN_F_CSUM) {
		features |= NETIF_F_HW_CSUM|NETIF_F_SG|NETIF_F_FRAGLIST;
		arg &= ~TUN_F_CSUM;

		if (arg & (TUN_F_TSO4|TUN_F_TSO6)) {
			if (arg & TUN_F_TSO_ECN) {
				features |= NETIF_F_TSO_ECN;
				arg &= ~TUN_F_TSO_ECN;
			}
			if (arg & TUN_F_TSO4)
				features |= NETIF_F_TSO;
			if (arg & TUN_F_TSO6)
				features |= NETIF_F_TSO6;
			arg &= ~(TUN_F_TSO4|TUN_F_TSO6);
		}
	}

	/* This gives the user a way to test for new features in future by
	 * trying to set them. */
	if (arg)
		return -EINVAL;

	dev->features = features;
	if (old_features != dev->features)
		netdev_features_change(dev);

	return 0;
}
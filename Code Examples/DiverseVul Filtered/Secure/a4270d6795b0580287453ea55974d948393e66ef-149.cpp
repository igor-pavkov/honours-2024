static void dev_disable_gro_hw(struct net_device *dev)
{
	dev->wanted_features &= ~NETIF_F_GRO_HW;
	netdev_update_features(dev);

	if (unlikely(dev->features & NETIF_F_GRO_HW))
		netdev_WARN(dev, "failed to disable GRO_HW!\n");
}
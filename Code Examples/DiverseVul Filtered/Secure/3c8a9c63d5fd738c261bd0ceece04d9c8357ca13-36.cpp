static void tun_setup(struct net_device *dev)
{
	struct tun_struct *tun = netdev_priv(dev);

	skb_queue_head_init(&tun->readq);

	tun->owner = -1;
	tun->group = -1;

	dev->ethtool_ops = &tun_ethtool_ops;
	dev->destructor = tun_free_netdev;
}
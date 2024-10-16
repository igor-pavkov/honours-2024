static void ip6gre_tap_setup(struct net_device *dev)
{

	ether_setup(dev);

	dev->netdev_ops = &ip6gre_tap_netdev_ops;
	dev->destructor = ip6gre_dev_free;

	dev->features |= NETIF_F_NETNS_LOCAL;
	dev->priv_flags &= ~IFF_TX_SKB_SHARING;
	dev->priv_flags |= IFF_LIVE_ADDR_CHANGE;
}
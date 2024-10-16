static void ip6gre_tunnel_setup(struct net_device *dev)
{
	dev->netdev_ops = &ip6gre_netdev_ops;
	dev->destructor = ip6gre_dev_free;

	dev->type = ARPHRD_IP6GRE;

	dev->flags |= IFF_NOARP;
	dev->addr_len = sizeof(struct in6_addr);
	netif_keep_dst(dev);
}
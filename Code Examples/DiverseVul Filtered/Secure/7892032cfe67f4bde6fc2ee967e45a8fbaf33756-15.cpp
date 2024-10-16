static int ip6gre_tap_init(struct net_device *dev)
{
	struct ip6_tnl *tunnel;
	int ret;

	ret = ip6gre_tunnel_init_common(dev);
	if (ret)
		return ret;

	dev->priv_flags |= IFF_LIVE_ADDR_CHANGE;

	tunnel = netdev_priv(dev);

	ip6gre_tnl_link_config(tunnel, 1);

	return 0;
}
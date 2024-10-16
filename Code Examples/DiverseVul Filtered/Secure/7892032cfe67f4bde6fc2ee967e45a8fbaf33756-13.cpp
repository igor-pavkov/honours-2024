static int ip6gre_tunnel_init(struct net_device *dev)
{
	struct ip6_tnl *tunnel;
	int ret;

	ret = ip6gre_tunnel_init_common(dev);
	if (ret)
		return ret;

	tunnel = netdev_priv(dev);

	memcpy(dev->dev_addr, &tunnel->parms.laddr, sizeof(struct in6_addr));
	memcpy(dev->broadcast, &tunnel->parms.raddr, sizeof(struct in6_addr));

	if (ipv6_addr_any(&tunnel->parms.raddr))
		dev->header_ops = &ip6gre_header_ops;

	return 0;
}
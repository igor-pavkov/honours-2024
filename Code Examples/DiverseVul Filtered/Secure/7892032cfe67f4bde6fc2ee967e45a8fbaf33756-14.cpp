static void ip6gre_fb_tunnel_init(struct net_device *dev)
{
	struct ip6_tnl *tunnel = netdev_priv(dev);

	tunnel->dev = dev;
	tunnel->net = dev_net(dev);
	strcpy(tunnel->parms.name, dev->name);

	tunnel->hlen		= sizeof(struct ipv6hdr) + 4;

	dev_hold(dev);
}
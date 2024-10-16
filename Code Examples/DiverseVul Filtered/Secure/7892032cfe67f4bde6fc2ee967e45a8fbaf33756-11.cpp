static void ip6gre_tunnel_uninit(struct net_device *dev)
{
	struct ip6_tnl *t = netdev_priv(dev);
	struct ip6gre_net *ign = net_generic(t->net, ip6gre_net_id);

	ip6gre_tunnel_unlink(ign, t);
	dst_cache_reset(&t->dst_cache);
	dev_put(dev);
}
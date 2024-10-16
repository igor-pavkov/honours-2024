static void ip6gre_dev_free(struct net_device *dev)
{
	struct ip6_tnl *t = netdev_priv(dev);

	dst_cache_destroy(&t->dst_cache);
	free_percpu(dev->tstats);
	free_netdev(dev);
}
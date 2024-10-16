static struct net_device_stats *ipip6_tunnel_get_stats(struct net_device *dev)
{
	return &(((struct ip_tunnel*)netdev_priv(dev))->stat);
}
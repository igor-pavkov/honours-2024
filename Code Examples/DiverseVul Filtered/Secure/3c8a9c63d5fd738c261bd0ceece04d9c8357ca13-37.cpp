static int tun_net_open(struct net_device *dev)
{
	netif_start_queue(dev);
	return 0;
}
static int tun_net_close(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}
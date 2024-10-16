static void ip6gre_dellink(struct net_device *dev, struct list_head *head)
{
	struct net *net = dev_net(dev);
	struct ip6gre_net *ign = net_generic(net, ip6gre_net_id);

	if (dev != ign->fb_tunnel_dev)
		unregister_netdevice_queue(dev, head);
}
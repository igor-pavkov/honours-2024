static void ipip6_tunnel_setup(struct net_device *dev)
{
	dev->uninit		= ipip6_tunnel_uninit;
	dev->destructor 	= free_netdev;
	dev->hard_start_xmit	= ipip6_tunnel_xmit;
	dev->get_stats		= ipip6_tunnel_get_stats;
	dev->do_ioctl		= ipip6_tunnel_ioctl;
	dev->change_mtu		= ipip6_tunnel_change_mtu;

	dev->type		= ARPHRD_SIT;
	dev->hard_header_len 	= LL_MAX_HEADER + sizeof(struct iphdr);
	dev->mtu		= ETH_DATA_LEN - sizeof(struct iphdr);
	dev->flags		= IFF_NOARP;
	dev->iflink		= 0;
	dev->addr_len		= 4;
	dev->features		|= NETIF_F_NETNS_LOCAL;
}
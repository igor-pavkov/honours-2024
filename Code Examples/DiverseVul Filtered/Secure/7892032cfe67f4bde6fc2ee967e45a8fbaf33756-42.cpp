static int ip6gre_tunnel_init_common(struct net_device *dev)
{
	struct ip6_tnl *tunnel;
	int ret;
	int t_hlen;

	tunnel = netdev_priv(dev);

	tunnel->dev = dev;
	tunnel->net = dev_net(dev);
	strcpy(tunnel->parms.name, dev->name);

	dev->tstats = netdev_alloc_pcpu_stats(struct pcpu_sw_netstats);
	if (!dev->tstats)
		return -ENOMEM;

	ret = dst_cache_init(&tunnel->dst_cache, GFP_KERNEL);
	if (ret) {
		free_percpu(dev->tstats);
		dev->tstats = NULL;
		return ret;
	}

	tunnel->tun_hlen = gre_calc_hlen(tunnel->parms.o_flags);
	tunnel->hlen = tunnel->tun_hlen + tunnel->encap_hlen;
	t_hlen = tunnel->hlen + sizeof(struct ipv6hdr);

	dev->hard_header_len = LL_MAX_HEADER + t_hlen;
	dev->mtu = ETH_DATA_LEN - t_hlen;
	if (dev->type == ARPHRD_ETHER)
		dev->mtu -= ETH_HLEN;
	if (!(tunnel->parms.flags & IP6_TNL_F_IGN_ENCAP_LIMIT))
		dev->mtu -= 8;

	return 0;
}
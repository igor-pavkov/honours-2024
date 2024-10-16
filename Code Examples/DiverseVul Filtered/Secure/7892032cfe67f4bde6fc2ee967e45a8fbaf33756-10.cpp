static struct ip6_tnl *ip6gre_tunnel_locate(struct net *net,
		const struct __ip6_tnl_parm *parms, int create)
{
	struct ip6_tnl *t, *nt;
	struct net_device *dev;
	char name[IFNAMSIZ];
	struct ip6gre_net *ign = net_generic(net, ip6gre_net_id);

	t = ip6gre_tunnel_find(net, parms, ARPHRD_IP6GRE);
	if (t && create)
		return NULL;
	if (t || !create)
		return t;

	if (parms->name[0])
		strlcpy(name, parms->name, IFNAMSIZ);
	else
		strcpy(name, "ip6gre%d");

	dev = alloc_netdev(sizeof(*t), name, NET_NAME_UNKNOWN,
			   ip6gre_tunnel_setup);
	if (!dev)
		return NULL;

	dev_net_set(dev, net);

	nt = netdev_priv(dev);
	nt->parms = *parms;
	dev->rtnl_link_ops = &ip6gre_link_ops;

	nt->dev = dev;
	nt->net = dev_net(dev);
	ip6gre_tnl_link_config(nt, 1);

	if (register_netdevice(dev) < 0)
		goto failed_free;

	/* Can use a lockless transmit, unless we generate output sequences */
	if (!(nt->parms.o_flags & TUNNEL_SEQ))
		dev->features |= NETIF_F_LLTX;

	dev_hold(dev);
	ip6gre_tunnel_link(ign, nt);
	return nt;

failed_free:
	free_netdev(dev);
	return NULL;
}
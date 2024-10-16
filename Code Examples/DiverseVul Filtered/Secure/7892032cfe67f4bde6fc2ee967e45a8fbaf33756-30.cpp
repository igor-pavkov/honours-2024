static int ip6gre_newlink(struct net *src_net, struct net_device *dev,
	struct nlattr *tb[], struct nlattr *data[])
{
	struct ip6_tnl *nt;
	struct net *net = dev_net(dev);
	struct ip6gre_net *ign = net_generic(net, ip6gre_net_id);
	struct ip_tunnel_encap ipencap;
	int err;

	nt = netdev_priv(dev);

	if (ip6gre_netlink_encap_parms(data, &ipencap)) {
		int err = ip6_tnl_encap_setup(nt, &ipencap);

		if (err < 0)
			return err;
	}

	ip6gre_netlink_parms(data, &nt->parms);

	if (ip6gre_tunnel_find(net, &nt->parms, dev->type))
		return -EEXIST;

	if (dev->type == ARPHRD_ETHER && !tb[IFLA_ADDRESS])
		eth_hw_addr_random(dev);

	nt->dev = dev;
	nt->net = dev_net(dev);
	ip6gre_tnl_link_config(nt, !tb[IFLA_MTU]);

	dev->features		|= GRE6_FEATURES;
	dev->hw_features	|= GRE6_FEATURES;

	if (!(nt->parms.o_flags & TUNNEL_SEQ)) {
		/* TCP offload with GRE SEQ is not supported, nor
		 * can we support 2 levels of outer headers requiring
		 * an update.
		 */
		if (!(nt->parms.o_flags & TUNNEL_CSUM) ||
		    (nt->encap.type == TUNNEL_ENCAP_NONE)) {
			dev->features    |= NETIF_F_GSO_SOFTWARE;
			dev->hw_features |= NETIF_F_GSO_SOFTWARE;
		}

		/* Can use a lockless transmit, unless we generate
		 * output sequences
		 */
		dev->features |= NETIF_F_LLTX;
	}

	err = register_netdevice(dev);
	if (err)
		goto out;

	dev_hold(dev);
	ip6gre_tunnel_link(ign, nt);

out:
	return err;
}
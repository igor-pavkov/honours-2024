static int ip6gre_changelink(struct net_device *dev, struct nlattr *tb[],
			    struct nlattr *data[])
{
	struct ip6_tnl *t, *nt = netdev_priv(dev);
	struct net *net = nt->net;
	struct ip6gre_net *ign = net_generic(net, ip6gre_net_id);
	struct __ip6_tnl_parm p;
	struct ip_tunnel_encap ipencap;

	if (dev == ign->fb_tunnel_dev)
		return -EINVAL;

	if (ip6gre_netlink_encap_parms(data, &ipencap)) {
		int err = ip6_tnl_encap_setup(nt, &ipencap);

		if (err < 0)
			return err;
	}

	ip6gre_netlink_parms(data, &p);

	t = ip6gre_tunnel_locate(net, &p, 0);

	if (t) {
		if (t->dev != dev)
			return -EEXIST;
	} else {
		t = nt;
	}

	ip6gre_tunnel_unlink(ign, t);
	ip6gre_tnl_change(t, &p, !tb[IFLA_MTU]);
	ip6gre_tunnel_link(ign, t);
	return 0;
}
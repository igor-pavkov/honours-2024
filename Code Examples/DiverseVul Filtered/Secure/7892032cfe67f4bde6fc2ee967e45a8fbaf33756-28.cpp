static struct ip6_tnl *ip6gre_tunnel_find(struct net *net,
					   const struct __ip6_tnl_parm *parms,
					   int type)
{
	const struct in6_addr *remote = &parms->raddr;
	const struct in6_addr *local = &parms->laddr;
	__be32 key = parms->i_key;
	int link = parms->link;
	struct ip6_tnl *t;
	struct ip6_tnl __rcu **tp;
	struct ip6gre_net *ign = net_generic(net, ip6gre_net_id);

	for (tp = __ip6gre_bucket(ign, parms);
	     (t = rtnl_dereference(*tp)) != NULL;
	     tp = &t->next)
		if (ipv6_addr_equal(local, &t->parms.laddr) &&
		    ipv6_addr_equal(remote, &t->parms.raddr) &&
		    key == t->parms.i_key &&
		    link == t->parms.link &&
		    type == t->dev->type)
			break;

	return t;
}
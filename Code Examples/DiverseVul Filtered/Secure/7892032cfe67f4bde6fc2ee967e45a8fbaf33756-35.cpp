static int ip6gre_header(struct sk_buff *skb, struct net_device *dev,
			unsigned short type,
			const void *daddr, const void *saddr, unsigned int len)
{
	struct ip6_tnl *t = netdev_priv(dev);
	struct ipv6hdr *ipv6h = (struct ipv6hdr *)skb_push(skb, t->hlen);
	__be16 *p = (__be16 *)(ipv6h+1);

	ip6_flow_hdr(ipv6h, 0,
		     ip6_make_flowlabel(dev_net(dev), skb,
					t->fl.u.ip6.flowlabel, true,
					&t->fl.u.ip6));
	ipv6h->hop_limit = t->parms.hop_limit;
	ipv6h->nexthdr = NEXTHDR_GRE;
	ipv6h->saddr = t->parms.laddr;
	ipv6h->daddr = t->parms.raddr;

	p[0]		= t->parms.o_flags;
	p[1]		= htons(type);

	/*
	 *	Set the source hardware address.
	 */

	if (saddr)
		memcpy(&ipv6h->saddr, saddr, sizeof(struct in6_addr));
	if (daddr)
		memcpy(&ipv6h->daddr, daddr, sizeof(struct in6_addr));
	if (!ipv6_addr_any(&ipv6h->daddr))
		return t->hlen;

	return -t->hlen;
}
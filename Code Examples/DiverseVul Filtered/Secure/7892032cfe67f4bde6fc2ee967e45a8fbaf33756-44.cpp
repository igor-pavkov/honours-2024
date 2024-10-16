static void ip6gre_tnl_link_config(struct ip6_tnl *t, int set_mtu)
{
	struct net_device *dev = t->dev;
	struct __ip6_tnl_parm *p = &t->parms;
	struct flowi6 *fl6 = &t->fl.u.ip6;
	int t_hlen;

	if (dev->type != ARPHRD_ETHER) {
		memcpy(dev->dev_addr, &p->laddr, sizeof(struct in6_addr));
		memcpy(dev->broadcast, &p->raddr, sizeof(struct in6_addr));
	}

	/* Set up flowi template */
	fl6->saddr = p->laddr;
	fl6->daddr = p->raddr;
	fl6->flowi6_oif = p->link;
	fl6->flowlabel = 0;
	fl6->flowi6_proto = IPPROTO_GRE;

	if (!(p->flags&IP6_TNL_F_USE_ORIG_TCLASS))
		fl6->flowlabel |= IPV6_TCLASS_MASK & p->flowinfo;
	if (!(p->flags&IP6_TNL_F_USE_ORIG_FLOWLABEL))
		fl6->flowlabel |= IPV6_FLOWLABEL_MASK & p->flowinfo;

	p->flags &= ~(IP6_TNL_F_CAP_XMIT|IP6_TNL_F_CAP_RCV|IP6_TNL_F_CAP_PER_PACKET);
	p->flags |= ip6_tnl_get_cap(t, &p->laddr, &p->raddr);

	if (p->flags&IP6_TNL_F_CAP_XMIT &&
			p->flags&IP6_TNL_F_CAP_RCV && dev->type != ARPHRD_ETHER)
		dev->flags |= IFF_POINTOPOINT;
	else
		dev->flags &= ~IFF_POINTOPOINT;

	t->tun_hlen = gre_calc_hlen(t->parms.o_flags);

	t->hlen = t->encap_hlen + t->tun_hlen;

	t_hlen = t->hlen + sizeof(struct ipv6hdr);

	if (p->flags & IP6_TNL_F_CAP_XMIT) {
		int strict = (ipv6_addr_type(&p->raddr) &
			      (IPV6_ADDR_MULTICAST|IPV6_ADDR_LINKLOCAL));

		struct rt6_info *rt = rt6_lookup(t->net,
						 &p->raddr, &p->laddr,
						 p->link, strict);

		if (!rt)
			return;

		if (rt->dst.dev) {
			dev->hard_header_len = rt->dst.dev->hard_header_len +
					       t_hlen;

			if (set_mtu) {
				dev->mtu = rt->dst.dev->mtu - t_hlen;
				if (!(t->parms.flags & IP6_TNL_F_IGN_ENCAP_LIMIT))
					dev->mtu -= 8;
				if (dev->type == ARPHRD_ETHER)
					dev->mtu -= ETH_HLEN;

				if (dev->mtu < IPV6_MIN_MTU)
					dev->mtu = IPV6_MIN_MTU;
			}
		}
		ip6_rt_put(rt);
	}
}
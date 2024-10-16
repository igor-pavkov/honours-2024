static inline int ip6gre_xmit_ipv6(struct sk_buff *skb, struct net_device *dev)
{
	struct ip6_tnl *t = netdev_priv(dev);
	struct ipv6hdr *ipv6h = ipv6_hdr(skb);
	int encap_limit = -1;
	__u16 offset;
	struct flowi6 fl6;
	__u8 dsfield;
	__u32 mtu;
	int err;

	if (ipv6_addr_equal(&t->parms.raddr, &ipv6h->saddr))
		return -1;

	offset = ip6_tnl_parse_tlv_enc_lim(skb, skb_network_header(skb));
	/* ip6_tnl_parse_tlv_enc_lim() might have reallocated skb->head */
	ipv6h = ipv6_hdr(skb);

	if (offset > 0) {
		struct ipv6_tlv_tnl_enc_lim *tel;
		tel = (struct ipv6_tlv_tnl_enc_lim *)&skb_network_header(skb)[offset];
		if (tel->encap_limit == 0) {
			icmpv6_send(skb, ICMPV6_PARAMPROB,
				    ICMPV6_HDR_FIELD, offset + 2);
			return -1;
		}
		encap_limit = tel->encap_limit - 1;
	} else if (!(t->parms.flags & IP6_TNL_F_IGN_ENCAP_LIMIT))
		encap_limit = t->parms.encap_limit;

	memcpy(&fl6, &t->fl.u.ip6, sizeof(fl6));

	dsfield = ipv6_get_dsfield(ipv6h);
	if (t->parms.flags & IP6_TNL_F_USE_ORIG_TCLASS)
		fl6.flowlabel |= (*(__be32 *) ipv6h & IPV6_TCLASS_MASK);
	if (t->parms.flags & IP6_TNL_F_USE_ORIG_FLOWLABEL)
		fl6.flowlabel |= ip6_flowlabel(ipv6h);
	if (t->parms.flags & IP6_TNL_F_USE_ORIG_FWMARK)
		fl6.flowi6_mark = skb->mark;

	fl6.flowi6_uid = sock_net_uid(dev_net(dev), NULL);

	if (gre_handle_offloads(skb, !!(t->parms.o_flags & TUNNEL_CSUM)))
		return -1;

	err = __gre6_xmit(skb, dev, dsfield, &fl6, encap_limit,
			  &mtu, skb->protocol);
	if (err != 0) {
		if (err == -EMSGSIZE)
			icmpv6_send(skb, ICMPV6_PKT_TOOBIG, 0, mtu);
		return -1;
	}

	return 0;
}
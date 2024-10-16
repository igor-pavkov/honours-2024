static inline int ip6gre_xmit_ipv4(struct sk_buff *skb, struct net_device *dev)
{
	struct ip6_tnl *t = netdev_priv(dev);
	const struct iphdr  *iph = ip_hdr(skb);
	int encap_limit = -1;
	struct flowi6 fl6;
	__u8 dsfield;
	__u32 mtu;
	int err;

	memset(&(IPCB(skb)->opt), 0, sizeof(IPCB(skb)->opt));

	if (!(t->parms.flags & IP6_TNL_F_IGN_ENCAP_LIMIT))
		encap_limit = t->parms.encap_limit;

	memcpy(&fl6, &t->fl.u.ip6, sizeof(fl6));

	dsfield = ipv4_get_dsfield(iph);

	if (t->parms.flags & IP6_TNL_F_USE_ORIG_TCLASS)
		fl6.flowlabel |= htonl((__u32)iph->tos << IPV6_TCLASS_SHIFT)
					  & IPV6_TCLASS_MASK;
	if (t->parms.flags & IP6_TNL_F_USE_ORIG_FWMARK)
		fl6.flowi6_mark = skb->mark;

	fl6.flowi6_uid = sock_net_uid(dev_net(dev), NULL);

	err = gre_handle_offloads(skb, !!(t->parms.o_flags & TUNNEL_CSUM));
	if (err)
		return -1;

	err = __gre6_xmit(skb, dev, dsfield, &fl6, encap_limit, &mtu,
			  skb->protocol);
	if (err != 0) {
		/* XXX: send ICMP error even if DF is not set. */
		if (err == -EMSGSIZE)
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
				  htonl(mtu));
		return -1;
	}

	return 0;
}
static netdev_tx_t __gre6_xmit(struct sk_buff *skb,
			       struct net_device *dev, __u8 dsfield,
			       struct flowi6 *fl6, int encap_limit,
			       __u32 *pmtu, __be16 proto)
{
	struct ip6_tnl *tunnel = netdev_priv(dev);
	__be16 protocol = (dev->type == ARPHRD_ETHER) ?
			  htons(ETH_P_TEB) : proto;

	if (dev->type == ARPHRD_ETHER)
		IPCB(skb)->flags = 0;

	if (dev->header_ops && dev->type == ARPHRD_IP6GRE)
		fl6->daddr = ((struct ipv6hdr *)skb->data)->daddr;
	else
		fl6->daddr = tunnel->parms.raddr;

	if (tunnel->parms.o_flags & TUNNEL_SEQ)
		tunnel->o_seqno++;

	/* Push GRE header. */
	gre_build_header(skb, tunnel->tun_hlen, tunnel->parms.o_flags,
			 protocol, tunnel->parms.o_key, htonl(tunnel->o_seqno));

	return ip6_tnl_xmit(skb, dev, dsfield, fl6, encap_limit, pmtu,
			    NEXTHDR_GRE);
}
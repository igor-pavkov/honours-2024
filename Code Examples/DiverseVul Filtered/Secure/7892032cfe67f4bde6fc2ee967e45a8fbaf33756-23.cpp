static int ip6gre_rcv(struct sk_buff *skb, const struct tnl_ptk_info *tpi)
{
	const struct ipv6hdr *ipv6h;
	struct ip6_tnl *tunnel;

	ipv6h = ipv6_hdr(skb);
	tunnel = ip6gre_tunnel_lookup(skb->dev,
				      &ipv6h->saddr, &ipv6h->daddr, tpi->key,
				      tpi->proto);
	if (tunnel) {
		ip6_tnl_rcv(tunnel, skb, tpi, NULL, false);

		return PACKET_RCVD;
	}

	return PACKET_REJECT;
}
static int gre_rcv(struct sk_buff *skb)
{
	struct tnl_ptk_info tpi;
	bool csum_err = false;
	int hdr_len;

	hdr_len = gre_parse_header(skb, &tpi, &csum_err, htons(ETH_P_IPV6), 0);
	if (hdr_len < 0)
		goto drop;

	if (iptunnel_pull_header(skb, hdr_len, tpi.proto, false))
		goto drop;

	if (ip6gre_rcv(skb, &tpi) == PACKET_RCVD)
		return 0;

	icmpv6_send(skb, ICMPV6_DEST_UNREACH, ICMPV6_PORT_UNREACH, 0);
drop:
	kfree_skb(skb);
	return 0;
}
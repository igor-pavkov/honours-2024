static int xfrm6_tunnel_output(struct xfrm_state *x, struct sk_buff *skb)
{
	struct ipv6hdr *top_iph;

	top_iph = (struct ipv6hdr *)skb->data;
	top_iph->payload_len = htons(skb->len - sizeof(struct ipv6hdr));

	return 0;
}
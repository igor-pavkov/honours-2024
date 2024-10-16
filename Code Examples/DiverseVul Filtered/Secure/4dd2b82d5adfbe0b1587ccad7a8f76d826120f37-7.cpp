INDIRECT_CALLABLE_SCOPE int udp4_gro_complete(struct sk_buff *skb, int nhoff)
{
	const struct iphdr *iph = ip_hdr(skb);
	struct udphdr *uh = (struct udphdr *)(skb->data + nhoff);

	if (uh->check)
		uh->check = ~udp_v4_check(skb->len - nhoff, iph->saddr,
					  iph->daddr, 0);

	return udp_gro_complete(skb, nhoff, udp4_lib_lookup_skb);
}
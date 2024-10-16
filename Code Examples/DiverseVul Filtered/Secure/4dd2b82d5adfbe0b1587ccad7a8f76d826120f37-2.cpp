struct sk_buff *udp4_gro_receive(struct list_head *head, struct sk_buff *skb)
{
	struct udphdr *uh = udp_gro_udphdr(skb);

	if (unlikely(!uh) || !static_branch_unlikely(&udp_encap_needed_key))
		goto flush;

	/* Don't bother verifying checksum if we're going to flush anyway. */
	if (NAPI_GRO_CB(skb)->flush)
		goto skip;

	if (skb_gro_checksum_validate_zero_check(skb, IPPROTO_UDP, uh->check,
						 inet_gro_compute_pseudo))
		goto flush;
	else if (uh->check)
		skb_gro_checksum_try_convert(skb, IPPROTO_UDP, uh->check,
					     inet_gro_compute_pseudo);
skip:
	NAPI_GRO_CB(skb)->is_ipv6 = 0;
	return udp_gro_receive(head, skb, uh, udp4_lib_lookup_skb);

flush:
	NAPI_GRO_CB(skb)->flush = 1;
	return NULL;
}
struct sk_buff *udp_gro_receive(struct list_head *head, struct sk_buff *skb,
				struct udphdr *uh, udp_lookup_t lookup)
{
	struct sk_buff *pp = NULL;
	struct sk_buff *p;
	struct udphdr *uh2;
	unsigned int off = skb_gro_offset(skb);
	int flush = 1;
	struct sock *sk;

	rcu_read_lock();
	sk = INDIRECT_CALL_INET(lookup, udp6_lib_lookup_skb,
				udp4_lib_lookup_skb, skb, uh->source, uh->dest);
	if (!sk)
		goto out_unlock;

	if (udp_sk(sk)->gro_enabled) {
		pp = call_gro_receive(udp_gro_receive_segment, head, skb);
		rcu_read_unlock();
		return pp;
	}

	if (NAPI_GRO_CB(skb)->encap_mark ||
	    (skb->ip_summed != CHECKSUM_PARTIAL &&
	     NAPI_GRO_CB(skb)->csum_cnt == 0 &&
	     !NAPI_GRO_CB(skb)->csum_valid) ||
	    !udp_sk(sk)->gro_receive)
		goto out_unlock;

	/* mark that this skb passed once through the tunnel gro layer */
	NAPI_GRO_CB(skb)->encap_mark = 1;

	flush = 0;

	list_for_each_entry(p, head, list) {
		if (!NAPI_GRO_CB(p)->same_flow)
			continue;

		uh2 = (struct udphdr   *)(p->data + off);

		/* Match ports and either checksums are either both zero
		 * or nonzero.
		 */
		if ((*(u32 *)&uh->source != *(u32 *)&uh2->source) ||
		    (!uh->check ^ !uh2->check)) {
			NAPI_GRO_CB(p)->same_flow = 0;
			continue;
		}
	}

	skb_gro_pull(skb, sizeof(struct udphdr)); /* pull encapsulating udp header */
	skb_gro_postpull_rcsum(skb, uh, sizeof(struct udphdr));
	pp = call_gro_receive_sk(udp_sk(sk)->gro_receive, sk, head, skb);

out_unlock:
	rcu_read_unlock();
	skb_gro_flush_final(skb, pp, flush);
	return pp;
}
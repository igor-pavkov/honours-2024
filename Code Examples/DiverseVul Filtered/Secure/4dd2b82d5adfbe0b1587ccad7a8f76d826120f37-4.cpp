int udp_gro_complete(struct sk_buff *skb, int nhoff,
		     udp_lookup_t lookup)
{
	__be16 newlen = htons(skb->len - nhoff);
	struct udphdr *uh = (struct udphdr *)(skb->data + nhoff);
	int err = -ENOSYS;
	struct sock *sk;

	uh->len = newlen;

	rcu_read_lock();
	sk = INDIRECT_CALL_INET(lookup, udp6_lib_lookup_skb,
				udp4_lib_lookup_skb, skb, uh->source, uh->dest);
	if (sk && udp_sk(sk)->gro_enabled) {
		err = udp_gro_complete_segment(skb);
	} else if (sk && udp_sk(sk)->gro_complete) {
		skb_shinfo(skb)->gso_type = uh->check ? SKB_GSO_UDP_TUNNEL_CSUM
					: SKB_GSO_UDP_TUNNEL;

		/* Set encapsulation before calling into inner gro_complete()
		 * functions to make them set up the inner offsets.
		 */
		skb->encapsulation = 1;
		err = udp_sk(sk)->gro_complete(sk, skb,
				nhoff + sizeof(struct udphdr));
	}
	rcu_read_unlock();

	if (skb->remcsum_offload)
		skb_shinfo(skb)->gso_type |= SKB_GSO_TUNNEL_REMCSUM;

	return err;
}
struct sk_buff *__udp_gso_segment(struct sk_buff *gso_skb,
				  netdev_features_t features)
{
	struct sock *sk = gso_skb->sk;
	unsigned int sum_truesize = 0;
	struct sk_buff *segs, *seg;
	struct udphdr *uh;
	unsigned int mss;
	bool copy_dtor;
	__sum16 check;
	__be16 newlen;

	mss = skb_shinfo(gso_skb)->gso_size;
	if (gso_skb->len <= sizeof(*uh) + mss)
		return ERR_PTR(-EINVAL);

	skb_pull(gso_skb, sizeof(*uh));

	/* clear destructor to avoid skb_segment assigning it to tail */
	copy_dtor = gso_skb->destructor == sock_wfree;
	if (copy_dtor)
		gso_skb->destructor = NULL;

	segs = skb_segment(gso_skb, features);
	if (unlikely(IS_ERR_OR_NULL(segs))) {
		if (copy_dtor)
			gso_skb->destructor = sock_wfree;
		return segs;
	}

	/* GSO partial and frag_list segmentation only requires splitting
	 * the frame into an MSS multiple and possibly a remainder, both
	 * cases return a GSO skb. So update the mss now.
	 */
	if (skb_is_gso(segs))
		mss *= skb_shinfo(segs)->gso_segs;

	seg = segs;
	uh = udp_hdr(seg);

	/* compute checksum adjustment based on old length versus new */
	newlen = htons(sizeof(*uh) + mss);
	check = csum16_add(csum16_sub(uh->check, uh->len), newlen);

	for (;;) {
		if (copy_dtor) {
			seg->destructor = sock_wfree;
			seg->sk = sk;
			sum_truesize += seg->truesize;
		}

		if (!seg->next)
			break;

		uh->len = newlen;
		uh->check = check;

		if (seg->ip_summed == CHECKSUM_PARTIAL)
			gso_reset_checksum(seg, ~check);
		else
			uh->check = gso_make_checksum(seg, ~check) ? :
				    CSUM_MANGLED_0;

		seg = seg->next;
		uh = udp_hdr(seg);
	}

	/* last packet can be partial gso_size, account for that in checksum */
	newlen = htons(skb_tail_pointer(seg) - skb_transport_header(seg) +
		       seg->data_len);
	check = csum16_add(csum16_sub(uh->check, uh->len), newlen);

	uh->len = newlen;
	uh->check = check;

	if (seg->ip_summed == CHECKSUM_PARTIAL)
		gso_reset_checksum(seg, ~check);
	else
		uh->check = gso_make_checksum(seg, ~check) ? : CSUM_MANGLED_0;

	/* update refcount for the packet */
	if (copy_dtor) {
		int delta = sum_truesize - gso_skb->truesize;

		/* In some pathological cases, delta can be negative.
		 * We need to either use refcount_add() or refcount_sub_and_test()
		 */
		if (likely(delta >= 0))
			refcount_add(delta, &sk->sk_wmem_alloc);
		else
			WARN_ON_ONCE(refcount_sub_and_test(-delta, &sk->sk_wmem_alloc));
	}
	return segs;
}
static struct sk_buff *pep_alloc_skb(struct sock *sk, const void *payload,
					int len, gfp_t priority)
{
	struct sk_buff *skb = alloc_skb(MAX_PNPIPE_HEADER + len, priority);
	if (!skb)
		return NULL;
	skb_set_owner_w(skb, sk);

	skb_reserve(skb, MAX_PNPIPE_HEADER);
	__skb_put(skb, len);
	skb_copy_to_linear_data(skb, payload, len);
	__skb_push(skb, sizeof(struct pnpipehdr));
	skb_reset_transport_header(skb);
	return skb;
}
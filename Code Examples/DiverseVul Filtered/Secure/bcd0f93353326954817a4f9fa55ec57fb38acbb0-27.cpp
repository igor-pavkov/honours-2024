int pep_write(struct sock *sk, struct sk_buff *skb)
{
	struct sk_buff *rskb, *fs;
	int flen = 0;

	if (pep_sk(sk)->aligned)
		return pipe_skb_send(sk, skb);

	rskb = alloc_skb(MAX_PNPIPE_HEADER, GFP_ATOMIC);
	if (!rskb) {
		kfree_skb(skb);
		return -ENOMEM;
	}
	skb_shinfo(rskb)->frag_list = skb;
	rskb->len += skb->len;
	rskb->data_len += rskb->len;
	rskb->truesize += rskb->len;

	/* Avoid nested fragments */
	skb_walk_frags(skb, fs)
		flen += fs->len;
	skb->next = skb_shinfo(skb)->frag_list;
	skb_frag_list_init(skb);
	skb->len -= flen;
	skb->data_len -= flen;
	skb->truesize -= flen;

	skb_reserve(rskb, MAX_PHONET_HEADER + 3);
	return pipe_skb_send(sk, rskb);
}
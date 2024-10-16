static inline void skb_orphan_try(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;

	if (sk && !skb_shinfo(skb)->tx_flags) {
		/* skb_tx_hash() wont be able to get sk.
		 * We copy sk_hash into skb->rxhash
		 */
		if (!skb->rxhash)
			skb->rxhash = sk->sk_hash;
		skb_orphan(skb);
	}
}
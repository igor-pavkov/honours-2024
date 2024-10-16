static void tcp_ack_tstamp(struct sock *sk, struct sk_buff *skb,
			   u32 prior_snd_una)
{
	const struct skb_shared_info *shinfo;

	/* Avoid cache line misses to get skb_shinfo() and shinfo->tx_flags */
	if (likely(!TCP_SKB_CB(skb)->txstamp_ack))
		return;

	shinfo = skb_shinfo(skb);
	if (!before(shinfo->tskey, prior_snd_una) &&
	    before(shinfo->tskey, tcp_sk(sk)->snd_una))
		__skb_tstamp_tx(skb, NULL, sk, SCM_TSTAMP_ACK);
}
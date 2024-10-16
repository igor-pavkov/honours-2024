static void ec_tx_done(struct sk_buff *skb, int result)
{
	struct ec_cb *eb = (struct ec_cb *)&skb->cb;
	tx_result(skb->sk, eb->cookie, result);
}
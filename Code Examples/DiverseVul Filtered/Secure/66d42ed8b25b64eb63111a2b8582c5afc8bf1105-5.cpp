static void ppp_tx_flush(void)
{
	struct sk_buff *skb;
	while ((skb = skb_dequeue(&tx_queue)) != NULL)
		dev_queue_xmit(skb);
}
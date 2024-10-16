static void tx_result(struct sock *sk, unsigned long cookie, int result)
{
	struct sk_buff *skb = alloc_skb(0, GFP_ATOMIC);
	struct ec_cb *eb;
	struct sockaddr_ec *sec;

	if (skb == NULL)
	{
		printk(KERN_DEBUG "ec: memory squeeze, transmit result dropped.\n");
		return;
	}

	eb = (struct ec_cb *)&skb->cb;
	sec = (struct sockaddr_ec *)&eb->sec;
	memset(sec, 0, sizeof(struct sockaddr_ec));
	sec->cookie = cookie;
	sec->type = ECTYPE_TRANSMIT_STATUS | result;
	sec->sec_family = AF_ECONET;

	if (sock_queue_rcv_skb(sk, skb) < 0)
		kfree_skb(skb);
}
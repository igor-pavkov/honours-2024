static void aun_tx_ack(unsigned long seq, int result)
{
	struct sk_buff *skb;
	unsigned long flags;
	struct ec_cb *eb;

	spin_lock_irqsave(&aun_queue_lock, flags);
	skb_queue_walk(&aun_queue, skb) {
		eb = (struct ec_cb *)&skb->cb;
		if (eb->seq == seq)
			goto foundit;
	}
	spin_unlock_irqrestore(&aun_queue_lock, flags);
	printk(KERN_DEBUG "AUN: unknown sequence %ld\n", seq);
	return;

foundit:
	tx_result(skb->sk, eb->cookie, result);
	skb_unlink(skb, &aun_queue);
	spin_unlock_irqrestore(&aun_queue_lock, flags);
	kfree_skb(skb);
}
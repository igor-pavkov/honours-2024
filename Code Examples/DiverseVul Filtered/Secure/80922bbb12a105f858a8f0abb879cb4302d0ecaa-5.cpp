static void ab_cleanup(unsigned long h)
{
	struct sk_buff *skb, *n;
	unsigned long flags;

	spin_lock_irqsave(&aun_queue_lock, flags);
	skb_queue_walk_safe(&aun_queue, skb, n) {
		struct ec_cb *eb = (struct ec_cb *)&skb->cb;
		if ((jiffies - eb->start) > eb->timeout) {
			tx_result(skb->sk, eb->cookie,
				  ECTYPE_TRANSMIT_NOT_PRESENT);
			skb_unlink(skb, &aun_queue);
			kfree_skb(skb);
		}
	}
	spin_unlock_irqrestore(&aun_queue_lock, flags);

	mod_timer(&ab_cleanup_timer, jiffies + (HZ*2));
}
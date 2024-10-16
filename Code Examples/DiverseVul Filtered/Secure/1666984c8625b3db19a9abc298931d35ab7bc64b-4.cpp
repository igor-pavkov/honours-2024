static void wait_skb_queue_empty(struct sk_buff_head *q)
{
	unsigned long flags;

	spin_lock_irqsave(&q->lock, flags);
	while (!skb_queue_empty(q)) {
		spin_unlock_irqrestore(&q->lock, flags);
		schedule_timeout(msecs_to_jiffies(UNLINK_TIMEOUT_MS));
		set_current_state(TASK_UNINTERRUPTIBLE);
		spin_lock_irqsave(&q->lock, flags);
	}
	spin_unlock_irqrestore(&q->lock, flags);
}
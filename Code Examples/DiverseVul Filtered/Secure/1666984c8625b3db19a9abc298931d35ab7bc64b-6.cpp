static enum skb_state defer_bh(struct usbnet *dev, struct sk_buff *skb,
		struct sk_buff_head *list, enum skb_state state)
{
	unsigned long		flags;
	enum skb_state 		old_state;
	struct skb_data *entry = (struct skb_data *) skb->cb;

	spin_lock_irqsave(&list->lock, flags);
	old_state = entry->state;
	entry->state = state;
	__skb_unlink(skb, list);

	/* defer_bh() is never called with list == &dev->done.
	 * spin_lock_nested() tells lockdep that it is OK to take
	 * dev->done.lock here with list->lock held.
	 */
	spin_lock_nested(&dev->done.lock, SINGLE_DEPTH_NESTING);

	__skb_queue_tail(&dev->done, skb);
	if (dev->done.qlen == 1)
		tasklet_schedule(&dev->bh);
	spin_unlock(&dev->done.lock);
	spin_unlock_irqrestore(&list->lock, flags);
	return old_state;
}
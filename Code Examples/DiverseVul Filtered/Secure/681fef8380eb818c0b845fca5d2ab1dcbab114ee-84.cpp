static struct async *async_getcompleted(struct usb_dev_state *ps)
{
	unsigned long flags;
	struct async *as = NULL;

	spin_lock_irqsave(&ps->lock, flags);
	if (!list_empty(&ps->async_completed)) {
		as = list_entry(ps->async_completed.next, struct async,
				asynclist);
		list_del_init(&as->asynclist);
	}
	spin_unlock_irqrestore(&ps->lock, flags);
	return as;
}
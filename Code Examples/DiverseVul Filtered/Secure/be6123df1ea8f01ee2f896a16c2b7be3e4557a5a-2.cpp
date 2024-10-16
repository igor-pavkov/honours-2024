static struct stub_unlink *dequeue_from_unlink_tx(struct stub_device *sdev)
{
	unsigned long flags;
	struct stub_unlink *unlink, *tmp;

	spin_lock_irqsave(&sdev->priv_lock, flags);

	list_for_each_entry_safe(unlink, tmp, &sdev->unlink_tx, list) {
		list_move_tail(&unlink->list, &sdev->unlink_free);
		spin_unlock_irqrestore(&sdev->priv_lock, flags);
		return unlink;
	}

	spin_unlock_irqrestore(&sdev->priv_lock, flags);

	return NULL;
}
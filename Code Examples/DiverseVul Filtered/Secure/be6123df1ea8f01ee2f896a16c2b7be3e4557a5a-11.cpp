static struct stub_priv *dequeue_from_priv_tx(struct stub_device *sdev)
{
	unsigned long flags;
	struct stub_priv *priv, *tmp;

	spin_lock_irqsave(&sdev->priv_lock, flags);

	list_for_each_entry_safe(priv, tmp, &sdev->priv_tx, list) {
		list_move_tail(&priv->list, &sdev->priv_free);
		spin_unlock_irqrestore(&sdev->priv_lock, flags);
		return priv;
	}

	spin_unlock_irqrestore(&sdev->priv_lock, flags);

	return NULL;
}
static struct b43_wldev * b43_wireless_core_stop(struct b43_wldev *dev)
{
	struct b43_wl *wl;
	struct b43_wldev *orig_dev;
	u32 mask;
	int queue_num;

	if (!dev)
		return NULL;
	wl = dev->wl;
redo:
	if (!dev || b43_status(dev) < B43_STAT_STARTED)
		return dev;

	/* Cancel work. Unlock to avoid deadlocks. */
	mutex_unlock(&wl->mutex);
	cancel_delayed_work_sync(&dev->periodic_work);
	cancel_work_sync(&wl->tx_work);
	mutex_lock(&wl->mutex);
	dev = wl->current_dev;
	if (!dev || b43_status(dev) < B43_STAT_STARTED) {
		/* Whoops, aliens ate up the device while we were unlocked. */
		return dev;
	}

	/* Disable interrupts on the device. */
	b43_set_status(dev, B43_STAT_INITIALIZED);
	if (b43_bus_host_is_sdio(dev->dev)) {
		/* wl->mutex is locked. That is enough. */
		b43_write32(dev, B43_MMIO_GEN_IRQ_MASK, 0);
		b43_read32(dev, B43_MMIO_GEN_IRQ_MASK);	/* Flush */
	} else {
		spin_lock_irq(&wl->hardirq_lock);
		b43_write32(dev, B43_MMIO_GEN_IRQ_MASK, 0);
		b43_read32(dev, B43_MMIO_GEN_IRQ_MASK);	/* Flush */
		spin_unlock_irq(&wl->hardirq_lock);
	}
	/* Synchronize and free the interrupt handlers. Unlock to avoid deadlocks. */
	orig_dev = dev;
	mutex_unlock(&wl->mutex);
	if (b43_bus_host_is_sdio(dev->dev)) {
		b43_sdio_free_irq(dev);
	} else {
		synchronize_irq(dev->dev->irq);
		free_irq(dev->dev->irq, dev);
	}
	mutex_lock(&wl->mutex);
	dev = wl->current_dev;
	if (!dev)
		return dev;
	if (dev != orig_dev) {
		if (b43_status(dev) >= B43_STAT_STARTED)
			goto redo;
		return dev;
	}
	mask = b43_read32(dev, B43_MMIO_GEN_IRQ_MASK);
	B43_WARN_ON(mask != 0xFFFFFFFF && mask);

	/* Drain all TX queues. */
	for (queue_num = 0; queue_num < B43_QOS_QUEUE_NUM; queue_num++) {
		while (skb_queue_len(&wl->tx_queue[queue_num])) {
			struct sk_buff *skb;

			skb = skb_dequeue(&wl->tx_queue[queue_num]);
			ieee80211_free_txskb(wl->hw, skb);
		}
	}

	b43_mac_suspend(dev);
	b43_leds_exit(dev);
	b43dbg(wl, "Wireless interface stopped\n");

	return dev;
}
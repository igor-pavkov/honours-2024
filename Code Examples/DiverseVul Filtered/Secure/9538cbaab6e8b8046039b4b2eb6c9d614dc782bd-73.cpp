static int b43_wireless_core_start(struct b43_wldev *dev)
{
	int err;

	B43_WARN_ON(b43_status(dev) != B43_STAT_INITIALIZED);

	drain_txstatus_queue(dev);
	if (b43_bus_host_is_sdio(dev->dev)) {
		err = b43_sdio_request_irq(dev, b43_sdio_interrupt_handler);
		if (err) {
			b43err(dev->wl, "Cannot request SDIO IRQ\n");
			goto out;
		}
	} else {
		err = request_threaded_irq(dev->dev->irq, b43_interrupt_handler,
					   b43_interrupt_thread_handler,
					   IRQF_SHARED, KBUILD_MODNAME, dev);
		if (err) {
			b43err(dev->wl, "Cannot request IRQ-%d\n",
			       dev->dev->irq);
			goto out;
		}
	}

	/* We are ready to run. */
	ieee80211_wake_queues(dev->wl->hw);
	b43_set_status(dev, B43_STAT_STARTED);

	/* Start data flow (TX/RX). */
	b43_mac_enable(dev);
	b43_write32(dev, B43_MMIO_GEN_IRQ_MASK, dev->irq_mask);

	/* Start maintenance work */
	b43_periodic_tasks_setup(dev);

	b43_leds_init(dev);

	b43dbg(dev->wl, "Wireless interface started\n");
out:
	return err;
}
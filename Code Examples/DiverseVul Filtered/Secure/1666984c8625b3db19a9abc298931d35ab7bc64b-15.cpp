static void usbnet_bh (unsigned long param)
{
	struct usbnet		*dev = (struct usbnet *) param;
	struct sk_buff		*skb;
	struct skb_data		*entry;

	while ((skb = skb_dequeue (&dev->done))) {
		entry = (struct skb_data *) skb->cb;
		switch (entry->state) {
		case rx_done:
			entry->state = rx_cleanup;
			rx_process (dev, skb);
			continue;
		case tx_done:
			kfree(entry->urb->sg);
		case rx_cleanup:
			usb_free_urb (entry->urb);
			dev_kfree_skb (skb);
			continue;
		default:
			netdev_dbg(dev->net, "bogus skb state %d\n", entry->state);
		}
	}

	/* restart RX again after disabling due to high error rate */
	clear_bit(EVENT_RX_KILL, &dev->flags);

	/* waiting for all pending urbs to complete?
	 * only then can we forgo submitting anew
	 */
	if (waitqueue_active(&dev->wait)) {
		if (dev->txq.qlen + dev->rxq.qlen + dev->done.qlen == 0)
			wake_up_all(&dev->wait);

	// or are we maybe short a few urbs?
	} else if (netif_running (dev->net) &&
		   netif_device_present (dev->net) &&
		   netif_carrier_ok(dev->net) &&
		   !timer_pending (&dev->delay) &&
		   !test_bit (EVENT_RX_HALT, &dev->flags)) {
		int	temp = dev->rxq.qlen;

		if (temp < RX_QLEN(dev)) {
			if (rx_alloc_submit(dev, GFP_ATOMIC) == -ENOLINK)
				return;
			if (temp != dev->rxq.qlen)
				netif_dbg(dev, link, dev->net,
					  "rxqlen %d --> %d\n",
					  temp, dev->rxq.qlen);
			if (dev->rxq.qlen < RX_QLEN(dev))
				tasklet_schedule (&dev->bh);
		}
		if (dev->txq.qlen < TX_QLEN (dev))
			netif_wake_queue (dev->net);
	}
}
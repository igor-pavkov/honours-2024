static inline void rx_process (struct usbnet *dev, struct sk_buff *skb)
{
	if (dev->driver_info->rx_fixup &&
	    !dev->driver_info->rx_fixup (dev, skb)) {
		/* With RX_ASSEMBLE, rx_fixup() must update counters */
		if (!(dev->driver_info->flags & FLAG_RX_ASSEMBLE))
			dev->net->stats.rx_errors++;
		goto done;
	}
	// else network stack removes extra byte if we forced a short packet

	/* all data was already cloned from skb inside the driver */
	if (dev->driver_info->flags & FLAG_MULTI_PACKET)
		goto done;

	if (skb->len < ETH_HLEN) {
		dev->net->stats.rx_errors++;
		dev->net->stats.rx_length_errors++;
		netif_dbg(dev, rx_err, dev->net, "rx length %d\n", skb->len);
	} else {
		usbnet_skb_return(dev, skb);
		return;
	}

done:
	skb_queue_tail(&dev->done, skb);
}
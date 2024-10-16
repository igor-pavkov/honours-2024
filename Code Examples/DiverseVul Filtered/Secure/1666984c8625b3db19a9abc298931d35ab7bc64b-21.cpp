static void tx_complete (struct urb *urb)
{
	struct sk_buff		*skb = (struct sk_buff *) urb->context;
	struct skb_data		*entry = (struct skb_data *) skb->cb;
	struct usbnet		*dev = entry->dev;

	if (urb->status == 0) {
		dev->net->stats.tx_packets += entry->packets;
		dev->net->stats.tx_bytes += entry->length;
	} else {
		dev->net->stats.tx_errors++;

		switch (urb->status) {
		case -EPIPE:
			usbnet_defer_kevent (dev, EVENT_TX_HALT);
			break;

		/* software-driven interface shutdown */
		case -ECONNRESET:		// async unlink
		case -ESHUTDOWN:		// hardware gone
			break;

		/* like rx, tx gets controller i/o faults during hub_wq
		 * delays and so it uses the same throttling mechanism.
		 */
		case -EPROTO:
		case -ETIME:
		case -EILSEQ:
			usb_mark_last_busy(dev->udev);
			if (!timer_pending (&dev->delay)) {
				mod_timer (&dev->delay,
					jiffies + THROTTLE_JIFFIES);
				netif_dbg(dev, link, dev->net,
					  "tx throttle %d\n", urb->status);
			}
			netif_stop_queue (dev->net);
			break;
		default:
			netif_dbg(dev, tx_err, dev->net,
				  "tx err %d\n", entry->urb->status);
			break;
		}
	}

	usb_autopm_put_interface_async(dev->intf);
	(void) defer_bh(dev, skb, &dev->txq, tx_done);
}
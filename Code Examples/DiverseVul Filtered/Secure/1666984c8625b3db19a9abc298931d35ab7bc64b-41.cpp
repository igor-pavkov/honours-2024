static int rx_submit (struct usbnet *dev, struct urb *urb, gfp_t flags)
{
	struct sk_buff		*skb;
	struct skb_data		*entry;
	int			retval = 0;
	unsigned long		lockflags;
	size_t			size = dev->rx_urb_size;

	/* prevent rx skb allocation when error ratio is high */
	if (test_bit(EVENT_RX_KILL, &dev->flags)) {
		usb_free_urb(urb);
		return -ENOLINK;
	}

	skb = __netdev_alloc_skb_ip_align(dev->net, size, flags);
	if (!skb) {
		netif_dbg(dev, rx_err, dev->net, "no rx skb\n");
		usbnet_defer_kevent (dev, EVENT_RX_MEMORY);
		usb_free_urb (urb);
		return -ENOMEM;
	}

	entry = (struct skb_data *) skb->cb;
	entry->urb = urb;
	entry->dev = dev;
	entry->length = 0;

	usb_fill_bulk_urb (urb, dev->udev, dev->in,
		skb->data, size, rx_complete, skb);

	spin_lock_irqsave (&dev->rxq.lock, lockflags);

	if (netif_running (dev->net) &&
	    netif_device_present (dev->net) &&
	    !test_bit (EVENT_RX_HALT, &dev->flags) &&
	    !test_bit (EVENT_DEV_ASLEEP, &dev->flags)) {
		switch (retval = usb_submit_urb (urb, GFP_ATOMIC)) {
		case -EPIPE:
			usbnet_defer_kevent (dev, EVENT_RX_HALT);
			break;
		case -ENOMEM:
			usbnet_defer_kevent (dev, EVENT_RX_MEMORY);
			break;
		case -ENODEV:
			netif_dbg(dev, ifdown, dev->net, "device gone\n");
			netif_device_detach (dev->net);
			break;
		case -EHOSTUNREACH:
			retval = -ENOLINK;
			break;
		default:
			netif_dbg(dev, rx_err, dev->net,
				  "rx submit, %d\n", retval);
			tasklet_schedule (&dev->bh);
			break;
		case 0:
			__usbnet_queue_skb(&dev->rxq, skb, rx_start);
		}
	} else {
		netif_dbg(dev, ifdown, dev->net, "rx: stopped\n");
		retval = -ENOLINK;
	}
	spin_unlock_irqrestore (&dev->rxq.lock, lockflags);
	if (retval) {
		dev_kfree_skb_any (skb);
		usb_free_urb (urb);
	}
	return retval;
}
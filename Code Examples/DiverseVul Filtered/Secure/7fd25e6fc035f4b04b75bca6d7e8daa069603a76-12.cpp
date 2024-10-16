static void atusb_in(struct urb *urb)
{
	struct usb_device *usb_dev = urb->dev;
	struct sk_buff *skb = urb->context;
	struct atusb *atusb = SKB_ATUSB(skb);

	dev_dbg(&usb_dev->dev, "%s: status %d len %d\n", __func__,
		urb->status, urb->actual_length);
	if (urb->status) {
		if (urb->status == -ENOENT) { /* being killed */
			kfree_skb(skb);
			urb->context = NULL;
			return;
		}
		dev_dbg(&usb_dev->dev, "%s: URB error %d\n", __func__, urb->status);
	} else {
		atusb_in_good(urb);
	}

	usb_anchor_urb(urb, &atusb->idle_urbs);
	if (!atusb->shutdown)
		schedule_delayed_work(&atusb->work, 0);
}
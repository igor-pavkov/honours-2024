static void atusb_in_good(struct urb *urb)
{
	struct usb_device *usb_dev = urb->dev;
	struct sk_buff *skb = urb->context;
	struct atusb *atusb = SKB_ATUSB(skb);
	u8 len, lqi;

	if (!urb->actual_length) {
		dev_dbg(&usb_dev->dev, "atusb_in: zero-sized URB ?\n");
		return;
	}

	len = *skb->data;

	if (urb->actual_length == 1) {
		atusb_tx_done(atusb, len);
		return;
	}

	if (len + 1 > urb->actual_length - 1) {
		dev_dbg(&usb_dev->dev, "atusb_in: frame len %d+1 > URB %u-1\n",
			len, urb->actual_length);
		return;
	}

	if (!ieee802154_is_valid_psdu_len(len)) {
		dev_dbg(&usb_dev->dev, "atusb_in: frame corrupted\n");
		return;
	}

	lqi = skb->data[len + 1];
	dev_dbg(&usb_dev->dev, "atusb_in: rx len %d lqi 0x%02x\n", len, lqi);
	skb_pull(skb, 1);	/* remove PHR */
	skb_trim(skb, len);	/* get payload only */
	ieee802154_rx_irqsafe(atusb->hw, skb, lqi);
	urb->context = NULL;	/* skb is gone */
}
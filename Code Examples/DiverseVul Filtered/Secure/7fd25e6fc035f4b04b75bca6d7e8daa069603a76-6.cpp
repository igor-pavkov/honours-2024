static int atusb_xmit(struct ieee802154_hw *hw, struct sk_buff *skb)
{
	struct atusb *atusb = hw->priv;
	struct usb_device *usb_dev = atusb->usb_dev;
	int ret;

	dev_dbg(&usb_dev->dev, "%s (%d)\n", __func__, skb->len);
	atusb->tx_skb = skb;
	atusb->tx_ack_seq++;
	atusb->tx_dr.wIndex = cpu_to_le16(atusb->tx_ack_seq);
	atusb->tx_dr.wLength = cpu_to_le16(skb->len);

	usb_fill_control_urb(atusb->tx_urb, usb_dev,
			     usb_sndctrlpipe(usb_dev, 0),
			     (unsigned char *)&atusb->tx_dr, skb->data,
			     skb->len, atusb_xmit_complete, NULL);
	ret = usb_submit_urb(atusb->tx_urb, GFP_ATOMIC);
	dev_dbg(&usb_dev->dev, "%s done (%d)\n", __func__, ret);
	return ret;
}
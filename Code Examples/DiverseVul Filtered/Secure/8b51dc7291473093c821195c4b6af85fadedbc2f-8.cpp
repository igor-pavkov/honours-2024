static int rsi_rx_urb_submit(struct rsi_hw *adapter, u8 ep_num)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	struct rx_usb_ctrl_block *rx_cb = &dev->rx_cb[ep_num - 1];
	struct urb *urb = rx_cb->rx_urb;
	int status;
	struct sk_buff *skb;
	u8 dword_align_bytes = 0;

#define RSI_MAX_RX_USB_PKT_SIZE	3000
	skb = dev_alloc_skb(RSI_MAX_RX_USB_PKT_SIZE);
	if (!skb)
		return -ENOMEM;
	skb_reserve(skb, MAX_DWORD_ALIGN_BYTES);
	skb_put(skb, RSI_MAX_RX_USB_PKT_SIZE - MAX_DWORD_ALIGN_BYTES);
	dword_align_bytes = (unsigned long)skb->data & 0x3f;
	if (dword_align_bytes > 0)
		skb_push(skb, dword_align_bytes);
	urb->transfer_buffer = skb->data;
	rx_cb->rx_skb = skb;

	usb_fill_bulk_urb(urb,
			  dev->usbdev,
			  usb_rcvbulkpipe(dev->usbdev,
			  dev->bulkin_endpoint_addr[ep_num - 1]),
			  urb->transfer_buffer,
			  skb->len,
			  rsi_rx_done_handler,
			  rx_cb);

	status = usb_submit_urb(urb, GFP_KERNEL);
	if (status)
		rsi_dbg(ERR_ZONE, "%s: Failed in urb submission\n", __func__);

	return status;
}
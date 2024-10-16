static void rsi_rx_done_handler(struct urb *urb)
{
	struct rx_usb_ctrl_block *rx_cb = urb->context;
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)rx_cb->data;
	int status = -EINVAL;

	if (urb->status) {
		dev_kfree_skb(rx_cb->rx_skb);
		return;
	}

	if (urb->actual_length <= 0 ||
	    urb->actual_length > rx_cb->rx_skb->len) {
		rsi_dbg(INFO_ZONE, "%s: Invalid packet length = %d\n",
			__func__, urb->actual_length);
		goto out;
	}
	if (skb_queue_len(&dev->rx_q) >= RSI_MAX_RX_PKTS) {
		rsi_dbg(INFO_ZONE, "Max RX packets reached\n");
		goto out;
	}
	skb_trim(rx_cb->rx_skb, urb->actual_length);
	skb_queue_tail(&dev->rx_q, rx_cb->rx_skb);

	rsi_set_event(&dev->rx_thread.event);
	status = 0;

out:
	if (rsi_rx_urb_submit(dev->priv, rx_cb->ep_num))
		rsi_dbg(ERR_ZONE, "%s: Failed in urb submission", __func__);

	if (status)
		dev_kfree_skb(rx_cb->rx_skb);
}
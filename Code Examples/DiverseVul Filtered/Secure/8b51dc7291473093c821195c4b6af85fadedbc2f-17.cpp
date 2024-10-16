static int rsi_usb_init_rx(struct rsi_hw *adapter)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	struct rx_usb_ctrl_block *rx_cb;
	u8 idx, num_rx_cb;

	num_rx_cb = (adapter->priv->coex_mode > 1 ? 2 : 1);

	for (idx = 0; idx < num_rx_cb; idx++) {
		rx_cb = &dev->rx_cb[idx];

		rx_cb->rx_urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!rx_cb->rx_urb) {
			rsi_dbg(ERR_ZONE, "Failed alloc rx urb[%d]\n", idx);
			goto err;
		}
		rx_cb->ep_num = idx + 1;
		rx_cb->data = (void *)dev;
	}
	skb_queue_head_init(&dev->rx_q);
	rsi_init_event(&dev->rx_thread.event);
	if (rsi_create_kthread(adapter->priv, &dev->rx_thread,
			       rsi_usb_rx_thread, "RX-Thread")) {
		rsi_dbg(ERR_ZONE, "%s: Unable to init rx thrd\n", __func__);
		goto err;
	}

	return 0;

err:
	usb_free_urb(dev->rx_cb[0].rx_urb);
	if (adapter->priv->coex_mode > 1)
		usb_free_urb(dev->rx_cb[1].rx_urb);

	return -1;
}
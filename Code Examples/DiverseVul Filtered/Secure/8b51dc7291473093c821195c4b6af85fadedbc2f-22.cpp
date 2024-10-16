static void rsi_deinit_usb_interface(struct rsi_hw *adapter)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;

	rsi_kill_thread(&dev->rx_thread);

	usb_free_urb(dev->rx_cb[0].rx_urb);
	if (adapter->priv->coex_mode > 1)
		usb_free_urb(dev->rx_cb[1].rx_urb);

	kfree(dev->tx_buffer);
}
static void mcba_urb_unlink(struct mcba_priv *priv)
{
	int i;

	usb_kill_anchored_urbs(&priv->rx_submitted);

	for (i = 0; i < MCBA_MAX_RX_URBS; ++i)
		usb_free_coherent(priv->udev, MCBA_USB_RX_BUFF_SIZE,
				  priv->rxbuf[i], priv->rxbuf_dma[i]);

	usb_kill_anchored_urbs(&priv->tx_submitted);
}
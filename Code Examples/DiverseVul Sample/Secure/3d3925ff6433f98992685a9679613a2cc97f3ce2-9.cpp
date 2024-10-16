static void unlink_all_urbs(struct usb_8dev_priv *priv)
{
	int i;

	usb_kill_anchored_urbs(&priv->rx_submitted);

	for (i = 0; i < MAX_RX_URBS; ++i)
		usb_free_coherent(priv->udev, RX_BUFFER_SIZE,
				  priv->rxbuf[i], priv->rxbuf_dma[i]);

	usb_kill_anchored_urbs(&priv->tx_submitted);
	atomic_set(&priv->active_tx_urbs, 0);

	for (i = 0; i < MAX_TX_URBS; i++)
		priv->tx_contexts[i].echo_index = MAX_TX_URBS;
}
static void usb_8dev_write_bulk_callback(struct urb *urb)
{
	struct usb_8dev_tx_urb_context *context = urb->context;
	struct usb_8dev_priv *priv;
	struct net_device *netdev;

	BUG_ON(!context);

	priv = context->priv;
	netdev = priv->netdev;

	/* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			  urb->transfer_buffer, urb->transfer_dma);

	atomic_dec(&priv->active_tx_urbs);

	if (!netif_device_present(netdev))
		return;

	if (urb->status)
		netdev_info(netdev, "Tx URB aborted (%d)\n",
			 urb->status);

	netdev->stats.tx_packets++;
	netdev->stats.tx_bytes += can_get_echo_skb(netdev, context->echo_index, NULL);

	can_led_event(netdev, CAN_LED_EVENT_TX);

	/* Release context */
	context->echo_index = MAX_TX_URBS;

	netif_wake_queue(netdev);
}
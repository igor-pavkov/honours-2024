static void usb_8dev_rx_can_msg(struct usb_8dev_priv *priv,
				struct usb_8dev_rx_msg *msg)
{
	struct can_frame *cf;
	struct sk_buff *skb;
	struct net_device_stats *stats = &priv->netdev->stats;

	if (msg->type == USB_8DEV_TYPE_ERROR_FRAME &&
		   msg->flags == USB_8DEV_ERR_FLAG) {
		usb_8dev_rx_err_msg(priv, msg);
	} else if (msg->type == USB_8DEV_TYPE_CAN_FRAME) {
		skb = alloc_can_skb(priv->netdev, &cf);
		if (!skb)
			return;

		cf->can_id = be32_to_cpu(msg->id);
		can_frame_set_cc_len(cf, msg->dlc & 0xF, priv->can.ctrlmode);

		if (msg->flags & USB_8DEV_EXTID)
			cf->can_id |= CAN_EFF_FLAG;

		if (msg->flags & USB_8DEV_RTR) {
			cf->can_id |= CAN_RTR_FLAG;
		} else {
			memcpy(cf->data, msg->data, cf->len);
			stats->rx_bytes += cf->len;
		}
		stats->rx_packets++;

		netif_rx(skb);

		can_led_event(priv->netdev, CAN_LED_EVENT_RX);
	} else {
		netdev_warn(priv->netdev, "frame type %d unknown",
			 msg->type);
	}

}
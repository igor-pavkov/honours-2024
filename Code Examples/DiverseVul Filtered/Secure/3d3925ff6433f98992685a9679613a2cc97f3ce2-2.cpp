static void usb_8dev_read_bulk_callback(struct urb *urb)
{
	struct usb_8dev_priv *priv = urb->context;
	struct net_device *netdev;
	int retval;
	int pos = 0;

	netdev = priv->netdev;

	if (!netif_device_present(netdev))
		return;

	switch (urb->status) {
	case 0: /* success */
		break;

	case -ENOENT:
	case -EPIPE:
	case -EPROTO:
	case -ESHUTDOWN:
		return;

	default:
		netdev_info(netdev, "Rx URB aborted (%d)\n",
			 urb->status);
		goto resubmit_urb;
	}

	while (pos < urb->actual_length) {
		struct usb_8dev_rx_msg *msg;

		if (pos + sizeof(struct usb_8dev_rx_msg) > urb->actual_length) {
			netdev_err(priv->netdev, "format error\n");
			break;
		}

		msg = (struct usb_8dev_rx_msg *)(urb->transfer_buffer + pos);
		usb_8dev_rx_can_msg(priv, msg);

		pos += sizeof(struct usb_8dev_rx_msg);
	}

resubmit_urb:
	usb_fill_bulk_urb(urb, priv->udev,
			  usb_rcvbulkpipe(priv->udev, USB_8DEV_ENDP_DATA_RX),
			  urb->transfer_buffer, RX_BUFFER_SIZE,
			  usb_8dev_read_bulk_callback, priv);

	retval = usb_submit_urb(urb, GFP_ATOMIC);

	if (retval == -ENODEV)
		netif_device_detach(netdev);
	else if (retval)
		netdev_err(netdev,
			"failed resubmitting read bulk urb: %d\n", retval);
}
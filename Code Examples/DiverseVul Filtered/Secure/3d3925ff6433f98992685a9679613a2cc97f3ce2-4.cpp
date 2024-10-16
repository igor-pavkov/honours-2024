static int usb_8dev_wait_cmd_msg(struct usb_8dev_priv *priv, u8 *msg, int size,
				int *actual_length)
{
	return usb_bulk_msg(priv->udev,
			    usb_rcvbulkpipe(priv->udev, USB_8DEV_ENDP_CMD_RX),
			    msg, size, actual_length, USB_8DEV_CMD_TIMEOUT);
}
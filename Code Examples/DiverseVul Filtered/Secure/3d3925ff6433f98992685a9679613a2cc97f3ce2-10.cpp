static int usb_8dev_send_cmd_msg(struct usb_8dev_priv *priv, u8 *msg, int size)
{
	int actual_length;

	return usb_bulk_msg(priv->udev,
			    usb_sndbulkpipe(priv->udev, USB_8DEV_ENDP_CMD_TX),
			    msg, size, &actual_length, USB_8DEV_CMD_TIMEOUT);
}
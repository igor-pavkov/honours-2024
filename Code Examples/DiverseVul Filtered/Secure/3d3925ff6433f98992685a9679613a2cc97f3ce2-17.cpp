static int usb_8dev_send_cmd(struct usb_8dev_priv *priv,
			     struct usb_8dev_cmd_msg *out,
			     struct usb_8dev_cmd_msg *in)
{
	int err;
	int num_bytes_read;
	struct net_device *netdev;

	netdev = priv->netdev;

	out->begin = USB_8DEV_CMD_START;
	out->end = USB_8DEV_CMD_END;

	mutex_lock(&priv->usb_8dev_cmd_lock);

	memcpy(priv->cmd_msg_buffer, out,
		sizeof(struct usb_8dev_cmd_msg));

	err = usb_8dev_send_cmd_msg(priv, priv->cmd_msg_buffer,
				    sizeof(struct usb_8dev_cmd_msg));
	if (err < 0) {
		netdev_err(netdev, "sending command message failed\n");
		goto failed;
	}

	err = usb_8dev_wait_cmd_msg(priv, priv->cmd_msg_buffer,
				    sizeof(struct usb_8dev_cmd_msg),
				    &num_bytes_read);
	if (err < 0) {
		netdev_err(netdev, "no command message answer\n");
		goto failed;
	}

	memcpy(in, priv->cmd_msg_buffer, sizeof(struct usb_8dev_cmd_msg));

	if (in->begin != USB_8DEV_CMD_START || in->end != USB_8DEV_CMD_END ||
			num_bytes_read != 16 || in->opt1 != 0)
		err = -EPROTO;

failed:
	mutex_unlock(&priv->usb_8dev_cmd_lock);
	return err;
}
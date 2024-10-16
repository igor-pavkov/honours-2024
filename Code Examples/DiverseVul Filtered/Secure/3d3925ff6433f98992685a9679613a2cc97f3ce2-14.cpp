static int usb_8dev_cmd_close(struct usb_8dev_priv *priv)
{
	struct usb_8dev_cmd_msg inmsg;
	struct usb_8dev_cmd_msg outmsg = {
		.channel = 0,
		.command = USB_8DEV_CLOSE,
		.opt1 = 0,
		.opt2 = 0
	};

	return usb_8dev_send_cmd(priv, &outmsg, &inmsg);
}
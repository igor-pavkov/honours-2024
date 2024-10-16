static int usb_8dev_cmd_version(struct usb_8dev_priv *priv, u32 *res)
{
	struct usb_8dev_cmd_msg	inmsg;
	struct usb_8dev_cmd_msg	outmsg = {
		.channel = 0,
		.command = USB_8DEV_GET_SOFTW_HARDW_VER,
		.opt1 = 0,
		.opt2 = 0
	};

	int err = usb_8dev_send_cmd(priv, &outmsg, &inmsg);
	if (err)
		return err;

	*res = be32_to_cpup((__be32 *)inmsg.data);

	return err;
}
static int usb_8dev_cmd_open(struct usb_8dev_priv *priv)
{
	struct can_bittiming *bt = &priv->can.bittiming;
	struct usb_8dev_cmd_msg outmsg;
	struct usb_8dev_cmd_msg inmsg;
	u32 ctrlmode = priv->can.ctrlmode;
	u32 flags = USB_8DEV_STATUS_FRAME;
	__be32 beflags;
	__be16 bebrp;

	memset(&outmsg, 0, sizeof(outmsg));
	outmsg.command = USB_8DEV_OPEN;
	outmsg.opt1 = USB_8DEV_BAUD_MANUAL;
	outmsg.data[0] = bt->prop_seg + bt->phase_seg1;
	outmsg.data[1] = bt->phase_seg2;
	outmsg.data[2] = bt->sjw;

	/* BRP */
	bebrp = cpu_to_be16((u16)bt->brp);
	memcpy(&outmsg.data[3], &bebrp, sizeof(bebrp));

	/* flags */
	if (ctrlmode & CAN_CTRLMODE_LOOPBACK)
		flags |= USB_8DEV_LOOPBACK;
	if (ctrlmode & CAN_CTRLMODE_LISTENONLY)
		flags |= USB_8DEV_SILENT;
	if (ctrlmode & CAN_CTRLMODE_ONE_SHOT)
		flags |= USB_8DEV_DISABLE_AUTO_RESTRANS;

	beflags = cpu_to_be32(flags);
	memcpy(&outmsg.data[5], &beflags, sizeof(beflags));

	return usb_8dev_send_cmd(priv, &outmsg, &inmsg);
}
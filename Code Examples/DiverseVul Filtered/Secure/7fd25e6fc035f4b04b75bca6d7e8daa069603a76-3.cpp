static int atusb_command(struct atusb *atusb, u8 cmd, u8 arg)
{
	struct usb_device *usb_dev = atusb->usb_dev;

	dev_dbg(&usb_dev->dev, "%s: cmd = 0x%x\n", __func__, cmd);
	return atusb_control_msg(atusb, usb_sndctrlpipe(usb_dev, 0),
				 cmd, ATUSB_REQ_TO_DEV, arg, 0, NULL, 0, 1000);
}
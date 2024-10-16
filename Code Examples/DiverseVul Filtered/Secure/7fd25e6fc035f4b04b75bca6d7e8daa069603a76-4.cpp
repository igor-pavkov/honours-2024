static int atusb_write_reg(struct atusb *atusb, u8 reg, u8 value)
{
	struct usb_device *usb_dev = atusb->usb_dev;

	dev_dbg(&usb_dev->dev, "%s: 0x%02x <- 0x%02x\n", __func__, reg, value);
	return atusb_control_msg(atusb, usb_sndctrlpipe(usb_dev, 0),
				 ATUSB_REG_WRITE, ATUSB_REQ_TO_DEV,
				 value, reg, NULL, 0, 1000);
}
static int atusb_read_reg(struct atusb *atusb, u8 reg)
{
	struct usb_device *usb_dev = atusb->usb_dev;
	int ret;
	u8 *buffer;
	u8 value;

	buffer = kmalloc(1, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	dev_dbg(&usb_dev->dev, "%s: reg = 0x%x\n", __func__, reg);
	ret = atusb_control_msg(atusb, usb_rcvctrlpipe(usb_dev, 0),
				ATUSB_REG_READ, ATUSB_REQ_FROM_DEV,
				0, reg, buffer, 1, 1000);

	if (ret >= 0) {
		value = buffer[0];
		kfree(buffer);
		return value;
	} else {
		kfree(buffer);
		return ret;
	}
}
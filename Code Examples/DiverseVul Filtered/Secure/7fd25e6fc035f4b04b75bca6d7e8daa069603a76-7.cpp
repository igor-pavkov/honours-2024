static int atusb_write_subreg(struct atusb *atusb, u8 reg, u8 mask,
			      u8 shift, u8 value)
{
	struct usb_device *usb_dev = atusb->usb_dev;
	u8 orig, tmp;
	int ret = 0;

	dev_dbg(&usb_dev->dev, "%s: 0x%02x <- 0x%02x\n", __func__, reg, value);

	orig = atusb_read_reg(atusb, reg);

	/* Write the value only into that part of the register which is allowed
	 * by the mask. All other bits stay as before.
	 */
	tmp = orig & ~mask;
	tmp |= (value << shift) & mask;

	if (tmp != orig)
		ret = atusb_write_reg(atusb, reg, tmp);

	return ret;
}
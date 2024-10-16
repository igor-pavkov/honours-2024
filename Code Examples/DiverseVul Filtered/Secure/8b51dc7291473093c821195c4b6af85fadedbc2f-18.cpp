static int rsi_usb_master_reg_read(struct rsi_hw *adapter, u32 reg,
				   u32 *value, u16 len)
{
	struct usb_device *usbdev =
		((struct rsi_91x_usbdev *)adapter->rsi_dev)->usbdev;
	u16 temp;
	int ret;

	ret = rsi_usb_reg_read(usbdev, reg, &temp, len);
	if (ret < 0)
		return ret;
	*value = temp;

	return 0;
}
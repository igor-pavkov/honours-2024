static int rsi_usb_master_reg_write(struct rsi_hw *adapter,
				    unsigned long reg,
				    unsigned long value, u16 len)
{
	struct usb_device *usbdev =
		((struct rsi_91x_usbdev *)adapter->rsi_dev)->usbdev;

	return rsi_usb_reg_write(usbdev, reg, value, len);
}
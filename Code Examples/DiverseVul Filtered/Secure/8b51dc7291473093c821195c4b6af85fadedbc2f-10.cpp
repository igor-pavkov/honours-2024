static int rsi_write_multiple(struct rsi_hw *adapter,
			      u8 endpoint,
			      u8 *data,
			      u32 count)
{
	struct rsi_91x_usbdev *dev;

	if (!adapter)
		return -ENODEV;

	if (endpoint == 0)
		return -EINVAL;

	dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	if (dev->write_fail)
		return -ENETDOWN;

	return rsi_usb_card_write(adapter, data, count, endpoint);
}
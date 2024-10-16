static int rsi_usb_read_register_multiple(struct rsi_hw *adapter, u32 addr,
					  u8 *data, u16 count)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	u8 *buf;
	u16 transfer;
	int status;

	if (!addr)
		return -EINVAL;

	buf = kzalloc(RSI_USB_BUF_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	while (count) {
		transfer = min_t(u16, count, RSI_USB_BUF_SIZE);
		status = usb_control_msg(dev->usbdev,
					 usb_rcvctrlpipe(dev->usbdev, 0),
					 USB_VENDOR_REGISTER_READ,
					 RSI_USB_REQ_IN,
					 ((addr & 0xffff0000) >> 16),
					 (addr & 0xffff), (void *)buf,
					 transfer, USB_CTRL_GET_TIMEOUT);
		if (status < 0) {
			rsi_dbg(ERR_ZONE,
				"Reg read failed with error code :%d\n",
				 status);
			kfree(buf);
			return status;
		}
		memcpy(data, buf, transfer);
		count -= transfer;
		data += transfer;
		addr += transfer;
	}
	kfree(buf);
	return 0;
}
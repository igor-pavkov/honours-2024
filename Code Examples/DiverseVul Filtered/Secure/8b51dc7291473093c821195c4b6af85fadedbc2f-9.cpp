static int rsi_usb_write_register_multiple(struct rsi_hw *adapter, u32 addr,
					   u8 *data, u16 count)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	u8 *buf;
	u16 transfer;
	int status = 0;

	buf = kzalloc(RSI_USB_BUF_SIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	while (count) {
		transfer = min_t(u16, count, RSI_USB_BUF_SIZE);
		memcpy(buf, data, transfer);
		status = usb_control_msg(dev->usbdev,
					 usb_sndctrlpipe(dev->usbdev, 0),
					 USB_VENDOR_REGISTER_WRITE,
					 RSI_USB_REQ_OUT,
					 ((addr & 0xffff0000) >> 16),
					 (addr & 0xffff),
					 (void *)buf,
					 transfer,
					 USB_CTRL_SET_TIMEOUT);
		if (status < 0) {
			rsi_dbg(ERR_ZONE,
				"Reg write failed with error code :%d\n",
				status);
			kfree(buf);
			return status;
		}
		count -= transfer;
		data += transfer;
		addr += transfer;
	}

	kfree(buf);
	return 0;
}
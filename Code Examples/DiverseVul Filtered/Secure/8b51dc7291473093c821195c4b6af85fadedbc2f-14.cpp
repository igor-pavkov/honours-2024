static int rsi_usb_reg_read(struct usb_device *usbdev,
			    u32 reg,
			    u16 *value,
			    u16 len)
{
	u8 *buf;
	int status = -ENOMEM;

	if (len > RSI_USB_CTRL_BUF_SIZE)
		return -EINVAL;

	buf  = kmalloc(RSI_USB_CTRL_BUF_SIZE, GFP_KERNEL);
	if (!buf)
		return status;

	status = usb_control_msg(usbdev,
				 usb_rcvctrlpipe(usbdev, 0),
				 USB_VENDOR_REGISTER_READ,
				 RSI_USB_REQ_IN,
				 ((reg & 0xffff0000) >> 16), (reg & 0xffff),
				 (void *)buf,
				 len,
				 USB_CTRL_GET_TIMEOUT);

	*value = (buf[0] | (buf[1] << 8));
	if (status < 0) {
		rsi_dbg(ERR_ZONE,
			"%s: Reg read failed with error code :%d\n",
			__func__, status);
	}
	kfree(buf);

	return status;
}
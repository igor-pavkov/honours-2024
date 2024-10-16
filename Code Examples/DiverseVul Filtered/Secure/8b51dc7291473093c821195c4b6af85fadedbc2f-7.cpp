static int rsi_usb_reg_write(struct usb_device *usbdev,
			     u32 reg,
			     u32 value,
			     u16 len)
{
	u8 *usb_reg_buf;
	int status = -ENOMEM;

	if (len > RSI_USB_CTRL_BUF_SIZE)
		return -EINVAL;

	usb_reg_buf  = kmalloc(RSI_USB_CTRL_BUF_SIZE, GFP_KERNEL);
	if (!usb_reg_buf)
		return status;

	usb_reg_buf[0] = (cpu_to_le32(value) & 0x00ff);
	usb_reg_buf[1] = (cpu_to_le32(value) & 0xff00) >> 8;
	usb_reg_buf[2] = (cpu_to_le32(value) & 0x00ff0000) >> 16;
	usb_reg_buf[3] = (cpu_to_le32(value) & 0xff000000) >> 24;

	status = usb_control_msg(usbdev,
				 usb_sndctrlpipe(usbdev, 0),
				 USB_VENDOR_REGISTER_WRITE,
				 RSI_USB_REQ_OUT,
				 ((cpu_to_le32(reg) & 0xffff0000) >> 16),
				 (cpu_to_le32(reg) & 0xffff),
				 (void *)usb_reg_buf,
				 len,
				 USB_CTRL_SET_TIMEOUT);
	if (status < 0) {
		rsi_dbg(ERR_ZONE,
			"%s: Reg write failed with error code :%d\n",
			__func__, status);
	}
	kfree(usb_reg_buf);

	return status;
}
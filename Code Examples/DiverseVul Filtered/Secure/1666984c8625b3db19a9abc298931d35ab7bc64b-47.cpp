int usbnet_write_cmd(struct usbnet *dev, u8 cmd, u8 reqtype,
		     u16 value, u16 index, const void *data, u16 size)
{
	int ret;

	if (usb_autopm_get_interface(dev->intf) < 0)
		return -ENODEV;
	ret = __usbnet_write_cmd(dev, cmd, reqtype, value, index,
				 data, size);
	usb_autopm_put_interface(dev->intf);
	return ret;
}
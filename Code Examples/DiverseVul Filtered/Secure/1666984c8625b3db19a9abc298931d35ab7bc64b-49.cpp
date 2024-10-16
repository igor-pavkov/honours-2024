int usbnet_read_cmd_nopm(struct usbnet *dev, u8 cmd, u8 reqtype,
			  u16 value, u16 index, void *data, u16 size)
{
	return __usbnet_read_cmd(dev, cmd, reqtype, value, index,
				 data, size);
}
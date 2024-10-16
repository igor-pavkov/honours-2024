int sisusb_setidxreg(struct sisusb_usb_data *sisusb, int port,
		u8 index, u8 data)
{
	int ret;

	ret = sisusb_write_memio_byte(sisusb, SISUSB_TYPE_IO, port, index);
	ret |= sisusb_write_memio_byte(sisusb, SISUSB_TYPE_IO, port + 1, data);
	return ret;
}
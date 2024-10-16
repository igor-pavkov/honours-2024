static int sisusb_setidxregmask(struct sisusb_usb_data *sisusb,
		int port, u8 idx, u8 data, u8 mask)
{
	int ret;
	u8 tmp;

	ret = sisusb_write_memio_byte(sisusb, SISUSB_TYPE_IO, port, idx);
	ret |= sisusb_read_memio_byte(sisusb, SISUSB_TYPE_IO, port + 1, &tmp);
	tmp &= ~(mask);
	tmp |= (data & mask);
	ret |= sisusb_write_memio_byte(sisusb, SISUSB_TYPE_IO, port + 1, tmp);
	return ret;
}
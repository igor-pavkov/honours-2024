int sisusb_setreg(struct sisusb_usb_data *sisusb, int port, u8 data)
{
	return sisusb_write_memio_byte(sisusb, SISUSB_TYPE_IO, port, data);
}
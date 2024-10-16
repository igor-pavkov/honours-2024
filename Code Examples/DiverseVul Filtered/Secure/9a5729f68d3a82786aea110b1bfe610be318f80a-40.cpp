int sisusb_readb(struct sisusb_usb_data *sisusb, u32 adr, u8 *data)
{
	return sisusb_read_memio_byte(sisusb, SISUSB_TYPE_MEM, adr, data);
}
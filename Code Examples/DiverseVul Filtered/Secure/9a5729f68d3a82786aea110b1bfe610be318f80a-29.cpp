int sisusb_writeb(struct sisusb_usb_data *sisusb, u32 adr, u8 data)
{
	return sisusb_write_memio_byte(sisusb, SISUSB_TYPE_MEM, adr, data);
}
static int sisusb_write_memio_byte(struct sisusb_usb_data *sisusb, int type,
		u32 addr, u8 data)
{
	struct sisusb_packet packet;

	packet.header  = (1 << (addr & 3)) | (type << 6);
	packet.address = addr & ~3;
	packet.data    = data << ((addr & 3) << 3);
	return sisusb_send_packet(sisusb, 10, &packet);
}
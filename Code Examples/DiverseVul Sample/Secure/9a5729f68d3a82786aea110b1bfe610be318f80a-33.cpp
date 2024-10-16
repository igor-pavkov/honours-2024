static int sisusb_read_memio_byte(struct sisusb_usb_data *sisusb, int type,
		u32 addr, u8 *data)
{
	struct sisusb_packet packet;
	int ret;

	CLEARPACKET(&packet);
	packet.header  = (1 << (addr & 3)) | (type << 6);
	packet.address = addr & ~3;
	ret = sisusb_send_packet(sisusb, 6, &packet);
	*data = (u8)(packet.data >> ((addr & 3) << 3));
	return ret;
}
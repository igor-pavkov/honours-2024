static int sisusb_write_memio_word(struct sisusb_usb_data *sisusb, int type,
		u32 addr, u16 data)
{
	struct sisusb_packet packet;
	int ret = 0;

	packet.address = addr & ~3;

	switch (addr & 3) {
	case 0:
		packet.header = (type << 6) | 0x0003;
		packet.data   = (u32)data;
		ret = sisusb_send_packet(sisusb, 10, &packet);
		break;
	case 1:
		packet.header = (type << 6) | 0x0006;
		packet.data   = (u32)data << 8;
		ret = sisusb_send_packet(sisusb, 10, &packet);
		break;
	case 2:
		packet.header = (type << 6) | 0x000c;
		packet.data   = (u32)data << 16;
		ret = sisusb_send_packet(sisusb, 10, &packet);
		break;
	case 3:
		packet.header = (type << 6) | 0x0008;
		packet.data   = (u32)data << 24;
		ret = sisusb_send_packet(sisusb, 10, &packet);
		packet.header = (type << 6) | 0x0001;
		packet.address = (addr & ~3) + 4;
		packet.data   = (u32)data >> 8;
		ret |= sisusb_send_packet(sisusb, 10, &packet);
	}

	return ret;
}
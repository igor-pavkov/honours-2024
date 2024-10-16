static int sisusb_read_memio_word(struct sisusb_usb_data *sisusb, int type,
		u32 addr, u16 *data)
{
	struct sisusb_packet packet;
	int ret = 0;

	CLEARPACKET(&packet);

	packet.address = addr & ~3;

	switch (addr & 3) {
	case 0:
		packet.header = (type << 6) | 0x0003;
		ret = sisusb_send_packet(sisusb, 6, &packet);
		*data = (u16)(packet.data);
		break;
	case 1:
		packet.header = (type << 6) | 0x0006;
		ret = sisusb_send_packet(sisusb, 6, &packet);
		*data = (u16)(packet.data >> 8);
		break;
	case 2:
		packet.header = (type << 6) | 0x000c;
		ret = sisusb_send_packet(sisusb, 6, &packet);
		*data = (u16)(packet.data >> 16);
		break;
	case 3:
		packet.header = (type << 6) | 0x0008;
		ret = sisusb_send_packet(sisusb, 6, &packet);
		*data = (u16)(packet.data >> 24);
		packet.header = (type << 6) | 0x0001;
		packet.address = (addr & ~3) + 4;
		ret |= sisusb_send_packet(sisusb, 6, &packet);
		*data |= (u16)(packet.data << 8);
	}

	return ret;
}
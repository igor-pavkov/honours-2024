static int sisusb_write_pci_config(struct sisusb_usb_data *sisusb,
		int regnum, u32 data)
{
	struct sisusb_packet packet;

	packet.header = 0x008f;
	packet.address = regnum | 0x10000;
	packet.data = data;
	return sisusb_send_packet(sisusb, 10, &packet);
}
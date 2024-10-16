static int sisusb_read_pci_config(struct sisusb_usb_data *sisusb,
		int regnum, u32 *data)
{
	struct sisusb_packet packet;
	int ret;

	packet.header = 0x008f;
	packet.address = (u32)regnum | 0x10000;
	ret = sisusb_send_packet(sisusb, 6, &packet);
	*data = packet.data;
	return ret;
}
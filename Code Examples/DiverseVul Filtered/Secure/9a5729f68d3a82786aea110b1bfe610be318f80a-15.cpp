int sisusb_setidxregand(struct sisusb_usb_data *sisusb, int port,
		u8 idx, u8 myand)
{
	return sisusb_setidxregandor(sisusb, port, idx, myand, 0x00);
}
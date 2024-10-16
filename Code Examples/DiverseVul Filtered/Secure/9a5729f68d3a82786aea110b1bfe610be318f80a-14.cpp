int sisusb_setidxregor(struct sisusb_usb_data *sisusb, int port,
		u8 index, u8 myor)
{
	return sisusb_setidxregandor(sisusb, port, index, 0xff, myor);
}
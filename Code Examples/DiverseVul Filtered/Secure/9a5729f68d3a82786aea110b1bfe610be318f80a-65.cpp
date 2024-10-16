static void sisusb_free_urbs(struct sisusb_usb_data *sisusb)
{
	int i;

	for (i = 0; i < NUMOBUFS; i++) {
		usb_free_urb(sisusb->sisurbout[i]);
		sisusb->sisurbout[i] = NULL;
	}
	usb_free_urb(sisusb->sisurbin);
	sisusb->sisurbin = NULL;
}
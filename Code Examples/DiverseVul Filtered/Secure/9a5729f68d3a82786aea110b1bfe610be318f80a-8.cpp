static int sisusb_alloc_outbuf(struct sisusb_usb_data *sisusb)
{
	int i;

	i = sisusb_outurb_available(sisusb);

	if (i >= 0)
		sisusb->urbstatus[i] |= SU_URB_ALLOC;

	return i;
}
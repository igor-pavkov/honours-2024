static void sisusb_free_outbuf(struct sisusb_usb_data *sisusb, int index)
{
	if ((index >= 0) && (index < sisusb->numobufs))
		sisusb->urbstatus[index] &= ~SU_URB_ALLOC;
}
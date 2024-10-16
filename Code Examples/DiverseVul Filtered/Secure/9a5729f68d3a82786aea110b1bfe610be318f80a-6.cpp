static int sisusb_outurb_available(struct sisusb_usb_data *sisusb)
{
	int i;

	for (i = 0; i < sisusb->numobufs; i++) {

		if ((sisusb->urbstatus[i] & (SU_URB_BUSY|SU_URB_ALLOC)) == 0)
			return i;

	}

	return -1;
}
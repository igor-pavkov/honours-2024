static int sisusb_all_free(struct sisusb_usb_data *sisusb)
{
	int i;

	for (i = 0; i < sisusb->numobufs; i++) {

		if (sisusb->urbstatus[i] & SU_URB_BUSY)
			return 0;

	}

	return 1;
}
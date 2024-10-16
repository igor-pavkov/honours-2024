static void sisusb_kill_all_busy(struct sisusb_usb_data *sisusb)
{
	int i;

	if (sisusb_all_free(sisusb))
		return;

	for (i = 0; i < sisusb->numobufs; i++) {

		if (sisusb->urbstatus[i] & SU_URB_BUSY)
			usb_kill_urb(sisusb->sisurbout[i]);

	}
}
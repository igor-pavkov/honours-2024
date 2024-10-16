static int sisusb_wait_all_out_complete(struct sisusb_usb_data *sisusb)
{
	int timeout = 5 * HZ, i = 1;

	wait_event_timeout(sisusb->wait_q, (i = sisusb_all_free(sisusb)),
			timeout);

	return i;
}
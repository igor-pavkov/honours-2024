static int sisusb_get_free_outbuf(struct sisusb_usb_data *sisusb)
{
	int i, timeout = 5 * HZ;

	wait_event_timeout(sisusb->wait_q,
			((i = sisusb_outurb_available(sisusb)) >= 0), timeout);

	return i;
}
static int sisusb_bulkin_msg(struct sisusb_usb_data *sisusb,
		unsigned int pipe, void *data, int len,
		int *actual_length, int timeout, unsigned int tflags)
{
	struct urb *urb = sisusb->sisurbin;
	int retval, readbytes = 0;

	urb->transfer_flags = 0;

	usb_fill_bulk_urb(urb, sisusb->sisusb_dev, pipe, data, len,
			sisusb_bulk_completein, sisusb);

	urb->transfer_flags |= tflags;
	urb->actual_length = 0;

	sisusb->completein = 0;
	retval = usb_submit_urb(urb, GFP_KERNEL);
	if (retval == 0) {
		wait_event_timeout(sisusb->wait_q, sisusb->completein, timeout);
		if (!sisusb->completein) {
			/* URB timed out... kill it and report error */
			usb_kill_urb(urb);
			retval = -ETIMEDOUT;
		} else {
			/* URB completed within timeout */
			retval = urb->status;
			readbytes = urb->actual_length;
		}
	}

	if (actual_length)
		*actual_length = readbytes;

	return retval;
}
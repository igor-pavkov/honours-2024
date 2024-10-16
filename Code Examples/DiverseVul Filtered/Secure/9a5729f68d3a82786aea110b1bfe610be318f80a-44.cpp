static int sisusb_bulkout_msg(struct sisusb_usb_data *sisusb, int index,
		unsigned int pipe, void *data, int len, int *actual_length,
		int timeout, unsigned int tflags)
{
	struct urb *urb = sisusb->sisurbout[index];
	int retval, byteswritten = 0;

	/* Set up URB */
	urb->transfer_flags = 0;

	usb_fill_bulk_urb(urb, sisusb->sisusb_dev, pipe, data, len,
			sisusb_bulk_completeout,
			&sisusb->urbout_context[index]);

	urb->transfer_flags |= tflags;
	urb->actual_length = 0;

	/* Set up context */
	sisusb->urbout_context[index].actual_length = (timeout) ?
			NULL : actual_length;

	/* Declare this urb/buffer in use */
	sisusb->urbstatus[index] |= SU_URB_BUSY;

	/* Submit URB */
	retval = usb_submit_urb(urb, GFP_KERNEL);

	/* If OK, and if timeout > 0, wait for completion */
	if ((retval == 0) && timeout) {
		wait_event_timeout(sisusb->wait_q,
				(!(sisusb->urbstatus[index] & SU_URB_BUSY)),
				timeout);
		if (sisusb->urbstatus[index] & SU_URB_BUSY) {
			/* URB timed out... kill it and report error */
			usb_kill_urb(urb);
			retval = -ETIMEDOUT;
		} else {
			/* Otherwise, report urb status */
			retval = urb->status;
			byteswritten = urb->actual_length;
		}
	}

	if (actual_length)
		*actual_length = byteswritten;

	return retval;
}
static int smsusb_submit_urb(struct smsusb_device_t *dev,
			     struct smsusb_urb_t *surb)
{
	if (!surb->cb) {
		/* This function can sleep */
		surb->cb = smscore_getbuffer(dev->coredev);
		if (!surb->cb) {
			pr_err("smscore_getbuffer(...) returned NULL\n");
			return -ENOMEM;
		}
	}

	usb_fill_bulk_urb(
		&surb->urb,
		dev->udev,
		usb_rcvbulkpipe(dev->udev, dev->in_ep),
		surb->cb->p,
		dev->buffer_size,
		smsusb_onresponse,
		surb
	);
	surb->urb.transfer_flags |= URB_FREE_BUFFER;

	return usb_submit_urb(&surb->urb, GFP_ATOMIC);
}
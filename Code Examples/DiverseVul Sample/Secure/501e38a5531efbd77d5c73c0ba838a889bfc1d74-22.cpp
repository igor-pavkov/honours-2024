static void ep0_complete (struct usb_ep *ep, struct usb_request *req)
{
	struct dev_data		*dev = ep->driver_data;
	unsigned long		flags;
	int			free = 1;

	/* for control OUT, data must still get to userspace */
	spin_lock_irqsave(&dev->lock, flags);
	if (!dev->setup_in) {
		dev->setup_out_error = (req->status != 0);
		if (!dev->setup_out_error)
			free = 0;
		dev->setup_out_ready = 1;
		ep0_readable (dev);
	}

	/* clean up as appropriate */
	if (free && req->buf != &dev->rbuf)
		clean_req (ep, req);
	req->complete = epio_complete;
	spin_unlock_irqrestore(&dev->lock, flags);
}
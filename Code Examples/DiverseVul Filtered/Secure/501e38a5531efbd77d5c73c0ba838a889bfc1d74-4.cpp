static void clean_req (struct usb_ep *ep, struct usb_request *req)
{
	struct dev_data		*dev = ep->driver_data;

	if (req->buf != dev->rbuf) {
		kfree(req->buf);
		req->buf = dev->rbuf;
	}
	req->complete = epio_complete;
	dev->setup_out_ready = 0;
}
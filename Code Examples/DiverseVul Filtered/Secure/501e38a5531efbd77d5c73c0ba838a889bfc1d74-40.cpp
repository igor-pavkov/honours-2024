static int setup_req (struct usb_ep *ep, struct usb_request *req, u16 len)
{
	struct dev_data	*dev = ep->driver_data;

	if (dev->setup_out_ready) {
		DBG (dev, "ep0 request busy!\n");
		return -EBUSY;
	}
	if (len > sizeof (dev->rbuf))
		req->buf = kmalloc(len, GFP_ATOMIC);
	if (req->buf == NULL) {
		req->buf = dev->rbuf;
		return -ENOMEM;
	}
	req->complete = ep0_complete;
	req->length = len;
	req->zero = 0;
	return 0;
}
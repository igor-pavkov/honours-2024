static void usbnet_async_cmd_cb(struct urb *urb)
{
	struct usb_ctrlrequest *req = (struct usb_ctrlrequest *)urb->context;
	int status = urb->status;

	if (status < 0)
		dev_dbg(&urb->dev->dev, "%s failed with %d",
			__func__, status);

	kfree(req);
	usb_free_urb(urb);
}
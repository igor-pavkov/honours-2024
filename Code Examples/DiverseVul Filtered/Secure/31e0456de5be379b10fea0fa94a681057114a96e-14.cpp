static int smsusb_resume(struct usb_interface *intf)
{
	int rc, i;
	struct smsusb_device_t *dev = usb_get_intfdata(intf);
	struct usb_device *udev = interface_to_usbdev(intf);

	printk(KERN_INFO "%s  Entering.\n", __func__);
	usb_clear_halt(udev, usb_rcvbulkpipe(udev, dev->in_ep));
	usb_clear_halt(udev, usb_sndbulkpipe(udev, dev->out_ep));

	for (i = 0; i < intf->cur_altsetting->desc.bNumEndpoints; i++)
		printk(KERN_INFO "endpoint %d %02x %02x %d\n", i,
		       intf->cur_altsetting->endpoint[i].desc.bEndpointAddress,
		       intf->cur_altsetting->endpoint[i].desc.bmAttributes,
		       intf->cur_altsetting->endpoint[i].desc.wMaxPacketSize);

	if (intf->num_altsetting > 0) {
		rc = usb_set_interface(udev,
				       intf->cur_altsetting->desc.
				       bInterfaceNumber, 0);
		if (rc < 0) {
			printk(KERN_INFO "%s usb_set_interface failed, rc %d\n",
			       __func__, rc);
			return rc;
		}
	}

	smsusb_start_streaming(dev);
	return 0;
}
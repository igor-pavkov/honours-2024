static void make_qualifier (struct dev_data *dev)
{
	struct usb_qualifier_descriptor		qual;
	struct usb_device_descriptor		*desc;

	qual.bLength = sizeof qual;
	qual.bDescriptorType = USB_DT_DEVICE_QUALIFIER;
	qual.bcdUSB = cpu_to_le16 (0x0200);

	desc = dev->dev;
	qual.bDeviceClass = desc->bDeviceClass;
	qual.bDeviceSubClass = desc->bDeviceSubClass;
	qual.bDeviceProtocol = desc->bDeviceProtocol;

	/* assumes ep0 uses the same value for both speeds ... */
	qual.bMaxPacketSize0 = dev->gadget->ep0->maxpacket;

	qual.bNumConfigurations = 1;
	qual.bRESERVED = 0;

	memcpy (dev->rbuf, &qual, sizeof qual);
}
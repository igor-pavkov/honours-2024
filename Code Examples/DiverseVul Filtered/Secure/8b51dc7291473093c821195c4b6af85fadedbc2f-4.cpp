static int rsi_find_bulk_in_and_out_endpoints(struct usb_interface *interface,
					      struct rsi_hw *adapter)
{
	struct rsi_91x_usbdev *dev = (struct rsi_91x_usbdev *)adapter->rsi_dev;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	__le16 buffer_size;
	int ii, bin_found = 0, bout_found = 0;

	iface_desc = &(interface->altsetting[0]);

	for (ii = 0; ii < iface_desc->desc.bNumEndpoints; ++ii) {
		endpoint = &(iface_desc->endpoint[ii].desc);

		if (!dev->bulkin_endpoint_addr[bin_found] &&
		    (endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		    USB_ENDPOINT_XFER_BULK)) {
			buffer_size = endpoint->wMaxPacketSize;
			dev->bulkin_size[bin_found] = buffer_size;
			dev->bulkin_endpoint_addr[bin_found] =
				endpoint->bEndpointAddress;
			bin_found++;
		}

		if (!dev->bulkout_endpoint_addr[bout_found] &&
		    !(endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		    USB_ENDPOINT_XFER_BULK)) {
			buffer_size = endpoint->wMaxPacketSize;
			dev->bulkout_endpoint_addr[bout_found] =
				endpoint->bEndpointAddress;
			dev->bulkout_size[bout_found] = buffer_size;
			bout_found++;
		}

		if (bin_found >= MAX_BULK_EP || bout_found >= MAX_BULK_EP)
			break;
	}

	if (!(dev->bulkin_endpoint_addr[0]) &&
	    dev->bulkout_endpoint_addr[0])
		return -EINVAL;

	return 0;
}
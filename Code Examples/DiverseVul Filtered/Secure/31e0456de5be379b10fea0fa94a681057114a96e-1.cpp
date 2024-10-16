static int smsusb_probe(struct usb_interface *intf,
			const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	char devpath[32];
	int i, rc;

	pr_info("board id=%lu, interface number %d\n",
		 id->driver_info,
		 intf->cur_altsetting->desc.bInterfaceNumber);

	if (sms_get_board(id->driver_info)->intf_num !=
	    intf->cur_altsetting->desc.bInterfaceNumber) {
		pr_debug("interface %d won't be used. Expecting interface %d to popup\n",
			intf->cur_altsetting->desc.bInterfaceNumber,
			sms_get_board(id->driver_info)->intf_num);
		return -ENODEV;
	}

	if (intf->num_altsetting > 1) {
		rc = usb_set_interface(udev,
				       intf->cur_altsetting->desc.bInterfaceNumber,
				       0);
		if (rc < 0) {
			pr_err("usb_set_interface failed, rc %d\n", rc);
			return rc;
		}
	}

	pr_debug("smsusb_probe %d\n",
	       intf->cur_altsetting->desc.bInterfaceNumber);
	for (i = 0; i < intf->cur_altsetting->desc.bNumEndpoints; i++) {
		pr_debug("endpoint %d %02x %02x %d\n", i,
		       intf->cur_altsetting->endpoint[i].desc.bEndpointAddress,
		       intf->cur_altsetting->endpoint[i].desc.bmAttributes,
		       intf->cur_altsetting->endpoint[i].desc.wMaxPacketSize);
		if (intf->cur_altsetting->endpoint[i].desc.bEndpointAddress &
		    USB_DIR_IN)
			rc = usb_clear_halt(udev, usb_rcvbulkpipe(udev,
				intf->cur_altsetting->endpoint[i].desc.bEndpointAddress));
		else
			rc = usb_clear_halt(udev, usb_sndbulkpipe(udev,
				intf->cur_altsetting->endpoint[i].desc.bEndpointAddress));
	}
	if ((udev->actconfig->desc.bNumInterfaces == 2) &&
	    (intf->cur_altsetting->desc.bInterfaceNumber == 0)) {
		pr_debug("rom interface 0 is not used\n");
		return -ENODEV;
	}

	if (id->driver_info == SMS1XXX_BOARD_SIANO_STELLAR_ROM) {
		/* Detected a Siano Stellar uninitialized */

		snprintf(devpath, sizeof(devpath), "usb\\%d-%s",
			 udev->bus->busnum, udev->devpath);
		pr_info("stellar device in cold state was found at %s.\n",
			devpath);
		rc = smsusb1_load_firmware(
				udev, smscore_registry_getmode(devpath),
				id->driver_info);

		/* This device will reset and gain another USB ID */
		if (!rc)
			pr_info("stellar device now in warm state\n");
		else
			pr_err("Failed to put stellar in warm state. Error: %d\n",
			       rc);

		return rc;
	} else {
		rc = smsusb_init_device(intf, id->driver_info);
	}

	pr_info("Device initialized with return code %d\n", rc);
	sms_board_load_modules(id->driver_info);
	return rc;
}
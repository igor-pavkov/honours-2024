static int atusb_get_and_show_revision(struct atusb *atusb)
{
	struct usb_device *usb_dev = atusb->usb_dev;
	char *hw_name;
	unsigned char *buffer;
	int ret;

	buffer = kmalloc(3, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	/* Get a couple of the ATMega Firmware values */
	ret = atusb_control_msg(atusb, usb_rcvctrlpipe(usb_dev, 0),
				ATUSB_ID, ATUSB_REQ_FROM_DEV, 0, 0,
				buffer, 3, 1000);
	if (ret >= 0) {
		atusb->fw_ver_maj = buffer[0];
		atusb->fw_ver_min = buffer[1];
		atusb->fw_hw_type = buffer[2];

		switch (atusb->fw_hw_type) {
		case ATUSB_HW_TYPE_100813:
		case ATUSB_HW_TYPE_101216:
		case ATUSB_HW_TYPE_110131:
			hw_name = "ATUSB";
			atusb->data = &atusb_chip_data;
			break;
		case ATUSB_HW_TYPE_RZUSB:
			hw_name = "RZUSB";
			atusb->data = &atusb_chip_data;
			break;
		case ATUSB_HW_TYPE_HULUSB:
			hw_name = "HULUSB";
			atusb->data = &hulusb_chip_data;
			break;
		default:
			hw_name = "UNKNOWN";
			atusb->err = -ENOTSUPP;
			ret = -ENOTSUPP;
			break;
		}

		dev_info(&usb_dev->dev,
			 "Firmware: major: %u, minor: %u, hardware type: %s (%d)\n",
			 atusb->fw_ver_maj, atusb->fw_ver_min, hw_name,
			 atusb->fw_hw_type);
	}
	if (atusb->fw_ver_maj == 0 && atusb->fw_ver_min < 2) {
		dev_info(&usb_dev->dev,
			 "Firmware version (%u.%u) predates our first public release.",
			 atusb->fw_ver_maj, atusb->fw_ver_min);
		dev_info(&usb_dev->dev, "Please update to version 0.2 or newer");
	}

	kfree(buffer);
	return ret;
}
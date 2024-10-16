int usb_get_status(struct usb_device *dev, int recip, int type, int target,
		void *data)
{
	int ret;
	void *status;
	int length;

	switch (type) {
	case USB_STATUS_TYPE_STANDARD:
		length = 2;
		break;
	case USB_STATUS_TYPE_PTM:
		if (recip != USB_RECIP_DEVICE)
			return -EINVAL;

		length = 4;
		break;
	default:
		return -EINVAL;
	}

	status =  kmalloc(length, GFP_KERNEL);
	if (!status)
		return -ENOMEM;

	ret = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
		USB_REQ_GET_STATUS, USB_DIR_IN | recip, USB_STATUS_TYPE_STANDARD,
		target, status, length, USB_CTRL_GET_TIMEOUT);

	switch (ret) {
	case 4:
		if (type != USB_STATUS_TYPE_PTM) {
			ret = -EIO;
			break;
		}

		*(u32 *) data = le32_to_cpu(*(__le32 *) status);
		ret = 0;
		break;
	case 2:
		if (type != USB_STATUS_TYPE_STANDARD) {
			ret = -EIO;
			break;
		}

		*(u16 *) data = le16_to_cpu(*(__le16 *) status);
		ret = 0;
		break;
	default:
		ret = -EIO;
	}

	kfree(status);
	return ret;
}
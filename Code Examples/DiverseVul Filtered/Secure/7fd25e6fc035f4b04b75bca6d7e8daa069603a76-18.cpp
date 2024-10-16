static int atusb_control_msg(struct atusb *atusb, unsigned int pipe,
			     __u8 request, __u8 requesttype,
			     __u16 value, __u16 index,
			     void *data, __u16 size, int timeout)
{
	struct usb_device *usb_dev = atusb->usb_dev;
	int ret;

	if (atusb->err)
		return atusb->err;

	ret = usb_control_msg(usb_dev, pipe, request, requesttype,
			      value, index, data, size, timeout);
	if (ret < 0) {
		atusb->err = ret;
		dev_err(&usb_dev->dev,
			"%s: req 0x%02x val 0x%x idx 0x%x, error %d\n",
			__func__, request, value, index, ret);
	}
	return ret;
}
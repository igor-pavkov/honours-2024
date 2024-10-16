static int read_packet(struct usb_device *udev,
		       u8 request, u8 * registers, u16 start, size_t size)
{
	unsigned char *buf;
	int ret;

	if (!registers || size <= 0)
		return -EINVAL;

	buf = kmalloc(size, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = usb_control_msg(udev,
			       usb_rcvctrlpipe(udev, 0),
			       request,
			       USB_DIR_IN|USB_TYPE_VENDOR|USB_RECIP_DEVICE,
			       start,	/* value */
			       0,	/* index */
			       buf,	/* buffer */
			       size,
			       HZ);

	if (ret >= 0)
		memcpy(registers, buf, size);

	kfree(buf);

	return ret;
}
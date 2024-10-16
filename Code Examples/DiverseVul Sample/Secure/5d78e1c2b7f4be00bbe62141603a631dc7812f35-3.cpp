int snd_usb_ctl_msg(struct usb_device *dev, unsigned int pipe, __u8 request,
		    __u8 requesttype, __u16 value, __u16 index, void *data,
		    __u16 size)
{
	int err;
	void *buf = NULL;
	int timeout;

	if (snd_usb_pipe_sanity_check(dev, pipe))
		return -EINVAL;

	if (size > 0) {
		buf = kmemdup(data, size, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;
	}

	if (requesttype & USB_DIR_IN)
		timeout = USB_CTRL_GET_TIMEOUT;
	else
		timeout = USB_CTRL_SET_TIMEOUT;

	err = usb_control_msg(dev, pipe, request, requesttype,
			      value, index, buf, size, timeout);

	if (size > 0) {
		memcpy(data, buf, size);
		kfree(buf);
	}

	snd_usb_ctl_msg_quirk(dev, pipe, request, requesttype,
			      value, index, data, size);

	return err;
}
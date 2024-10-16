static void snoop_urb(struct usb_device *udev,
		void __user *userurb, int pipe, unsigned length,
		int timeout_or_status, enum snoop_when when,
		unsigned char *data, unsigned data_len)
{
	static const char *types[] = {"isoc", "int", "ctrl", "bulk"};
	static const char *dirs[] = {"out", "in"};
	int ep;
	const char *t, *d;

	if (!usbfs_snoop)
		return;

	ep = usb_pipeendpoint(pipe);
	t = types[usb_pipetype(pipe)];
	d = dirs[!!usb_pipein(pipe)];

	if (userurb) {		/* Async */
		if (when == SUBMIT)
			dev_info(&udev->dev, "userurb %p, ep%d %s-%s, "
					"length %u\n",
					userurb, ep, t, d, length);
		else
			dev_info(&udev->dev, "userurb %p, ep%d %s-%s, "
					"actual_length %u status %d\n",
					userurb, ep, t, d, length,
					timeout_or_status);
	} else {
		if (when == SUBMIT)
			dev_info(&udev->dev, "ep%d %s-%s, length %u, "
					"timeout %d\n",
					ep, t, d, length, timeout_or_status);
		else
			dev_info(&udev->dev, "ep%d %s-%s, actual_length %u, "
					"status %d\n",
					ep, t, d, length, timeout_or_status);
	}

	data_len = min(data_len, usbfs_snoop_max);
	if (data && data_len > 0) {
		print_hex_dump(KERN_DEBUG, "data: ", DUMP_PREFIX_NONE, 32, 1,
			data, data_len, 1);
	}
}
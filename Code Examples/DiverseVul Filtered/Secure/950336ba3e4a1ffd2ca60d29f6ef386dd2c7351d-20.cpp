static int ati_remote2_setup(struct ati_remote2 *ar2, unsigned int ch_mask)
{
	int r, i, channel;

	/*
	 * Configure receiver to only accept input from remote "channel"
	 *  channel == 0  -> Accept input from any remote channel
	 *  channel == 1  -> Only accept input from remote channel 1
	 *  channel == 2  -> Only accept input from remote channel 2
	 *  ...
	 *  channel == 16 -> Only accept input from remote channel 16
	 */

	channel = 0;
	for (i = 0; i < 16; i++) {
		if ((1 << i) & ch_mask) {
			if (!(~(1 << i) & ch_mask))
				channel = i + 1;
			break;
		}
	}

	r = usb_control_msg(ar2->udev, usb_sndctrlpipe(ar2->udev, 0),
			    0x20,
			    USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE,
			    channel, 0x0, NULL, 0, USB_CTRL_SET_TIMEOUT);
	if (r) {
		dev_err(&ar2->udev->dev, "%s - failed to set channel due to error: %d\n",
			__func__, r);
		return r;
	}

	return 0;
}
static int airspy_ctrl_msg(struct airspy *s, u8 request, u16 value, u16 index,
		u8 *data, u16 size)
{
	int ret;
	unsigned int pipe;
	u8 requesttype;

	switch (request) {
	case CMD_RECEIVER_MODE:
	case CMD_SET_FREQ:
		pipe = usb_sndctrlpipe(s->udev, 0);
		requesttype = (USB_TYPE_VENDOR | USB_DIR_OUT);
		break;
	case CMD_BOARD_ID_READ:
	case CMD_VERSION_STRING_READ:
	case CMD_BOARD_PARTID_SERIALNO_READ:
	case CMD_SET_LNA_GAIN:
	case CMD_SET_MIXER_GAIN:
	case CMD_SET_VGA_GAIN:
	case CMD_SET_LNA_AGC:
	case CMD_SET_MIXER_AGC:
		pipe = usb_rcvctrlpipe(s->udev, 0);
		requesttype = (USB_TYPE_VENDOR | USB_DIR_IN);
		break;
	default:
		dev_err(s->dev, "Unknown command %02x\n", request);
		ret = -EINVAL;
		goto err;
	}

	/* write request */
	if (!(requesttype & USB_DIR_IN))
		memcpy(s->buf, data, size);

	ret = usb_control_msg(s->udev, pipe, request, requesttype, value,
			index, s->buf, size, 1000);
	airspy_dbg_usb_control_msg(s->dev, request, requesttype, value,
			index, s->buf, size);
	if (ret < 0) {
		dev_err(s->dev, "usb_control_msg() failed %d request %02x\n",
				ret, request);
		goto err;
	}

	/* read request */
	if (requesttype & USB_DIR_IN)
		memcpy(data, s->buf, size);

	return 0;
err:
	return ret;
}
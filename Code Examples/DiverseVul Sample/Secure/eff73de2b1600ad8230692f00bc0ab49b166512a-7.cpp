int cpia2_usb_transfer_cmd(struct camera_data *cam,
			   void *registers,
			   u8 request, u8 start, u8 count, u8 direction)
{
	int err = 0;
	struct usb_device *udev = cam->dev;

	if (!udev) {
		ERR("%s: Internal driver error: udev is NULL\n", __func__);
		return -EINVAL;
	}

	if (!registers) {
		ERR("%s: Internal driver error: register array is NULL\n", __func__);
		return -EINVAL;
	}

	if (direction == TRANSFER_READ) {
		err = read_packet(udev, request, (u8 *)registers, start, count);
		if (err > 0)
			err = 0;
	} else if (direction == TRANSFER_WRITE) {
		err =write_packet(udev, request, (u8 *)registers, start, count);
		if (err < 0) {
			LOG("Control message failed, err val = %d\n", err);
			LOG("Message: request = 0x%0X, start = 0x%0X\n",
			    request, start);
			LOG("Message: count = %d, register[0] = 0x%0X\n",
			    count, ((unsigned char *) registers)[0]);
		} else
			err=0;
	} else {
		LOG("Unexpected first byte of direction: %d\n",
		       direction);
		return -EINVAL;
	}

	if(err != 0)
		LOG("Unexpected error: %d\n", err);
	return err;
}
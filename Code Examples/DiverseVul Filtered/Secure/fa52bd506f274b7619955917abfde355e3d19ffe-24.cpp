static int usbvision_radio_open(struct file *file)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	int err_code = 0;

	PDEBUG(DBG_IO, "%s:", __func__);

	if (mutex_lock_interruptible(&usbvision->v4l2_lock))
		return -ERESTARTSYS;
	err_code = v4l2_fh_open(file);
	if (err_code)
		goto out;
	if (usbvision->user) {
		dev_err(&usbvision->rdev.dev,
			"%s: Someone tried to open an already opened USBVision Radio!\n",
				__func__);
		err_code = -EBUSY;
	} else {
		/* Alternate interface 1 is is the biggest frame size */
		err_code = usbvision_set_alternate(usbvision);
		if (err_code < 0) {
			usbvision->last_error = err_code;
			err_code = -EBUSY;
			goto out;
		}

		/* If so far no errors then we shall start the radio */
		usbvision->radio = 1;
		call_all(usbvision, tuner, s_radio);
		usbvision_set_audio(usbvision, USBVISION_AUDIO_RADIO);
		usbvision->user++;
	}
out:
	mutex_unlock(&usbvision->v4l2_lock);
	return err_code;
}
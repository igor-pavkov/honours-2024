static void usbvision_configure_video(struct usb_usbvision *usbvision)
{
	int model;

	if (usbvision == NULL)
		return;

	model = usbvision->dev_model;
	usbvision->palette = usbvision_v4l2_format[2]; /* V4L2_PIX_FMT_RGB24; */

	if (usbvision_device_data[usbvision->dev_model].vin_reg2_override) {
		usbvision->vin_reg2_preset =
			usbvision_device_data[usbvision->dev_model].vin_reg2;
	} else {
		usbvision->vin_reg2_preset = 0;
	}

	usbvision->tvnorm_id = usbvision_device_data[model].video_norm;
	usbvision->video_inputs = usbvision_device_data[model].video_channels;
	usbvision->ctl_input = 0;
	usbvision->radio_freq = 87.5 * 16000;
	usbvision->tv_freq = 400 * 16;

	/* This should be here to make i2c clients to be able to register */
	/* first switch off audio */
	if (usbvision_device_data[model].audio_channels > 0)
		usbvision_audio_off(usbvision);
	/* and then power up the tuner */
	usbvision_power_on(usbvision);
	usbvision_i2c_register(usbvision);
}
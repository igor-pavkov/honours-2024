static int vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *freq)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	/* Only one tuner */
	if (freq->tuner)
		return -EINVAL;
	if (freq->type == V4L2_TUNER_RADIO)
		freq->frequency = usbvision->radio_freq;
	else
		freq->frequency = usbvision->tv_freq;

	return 0;
}
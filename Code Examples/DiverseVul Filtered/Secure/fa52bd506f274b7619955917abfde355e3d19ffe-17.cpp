static int vidioc_s_frequency(struct file *file, void *priv,
				const struct v4l2_frequency *freq)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	struct v4l2_frequency new_freq = *freq;

	/* Only one tuner for now */
	if (freq->tuner)
		return -EINVAL;

	call_all(usbvision, tuner, s_frequency, freq);
	call_all(usbvision, tuner, g_frequency, &new_freq);
	if (freq->type == V4L2_TUNER_RADIO)
		usbvision->radio_freq = new_freq.frequency;
	else
		usbvision->tv_freq = new_freq.frequency;

	return 0;
}
static int vidioc_s_tuner(struct file *file, void *priv,
				const struct v4l2_tuner *vt)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	/* Only one tuner for now */
	if (vt->index)
		return -EINVAL;
	/* let clients handle this */
	call_all(usbvision, tuner, s_tuner, vt);

	return 0;
}
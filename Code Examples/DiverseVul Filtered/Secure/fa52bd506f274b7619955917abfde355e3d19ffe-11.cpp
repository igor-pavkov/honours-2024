static int vidioc_g_tuner(struct file *file, void *priv,
				struct v4l2_tuner *vt)
{
	struct usb_usbvision *usbvision = video_drvdata(file);

	if (vt->index)	/* Only tuner 0 */
		return -EINVAL;
	if (vt->type == V4L2_TUNER_RADIO)
		strcpy(vt->name, "Radio");
	else
		strcpy(vt->name, "Television");

	/* Let clients fill in the remainder of this struct */
	call_all(usbvision, tuner, g_tuner, vt);

	return 0;
}
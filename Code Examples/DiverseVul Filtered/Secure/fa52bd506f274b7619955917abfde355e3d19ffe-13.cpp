static ssize_t show_brightness(struct device *cd,
			       struct device_attribute *attr, char *buf)
{
	struct video_device *vdev =
		container_of(cd, struct video_device, dev);
	struct usb_usbvision *usbvision = video_get_drvdata(vdev);
	struct v4l2_control ctrl;
	ctrl.id = V4L2_CID_BRIGHTNESS;
	ctrl.value = 0;
	if (usbvision->user)
		call_all(usbvision, core, g_ctrl, &ctrl);
	return sprintf(buf, "%d\n", ctrl.value);
}
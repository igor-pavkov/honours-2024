static int vidioc_querycap(struct file *file, void  *priv,
					struct v4l2_capability *vc)
{
	struct usb_usbvision *usbvision = video_drvdata(file);
	struct video_device *vdev = video_devdata(file);

	strlcpy(vc->driver, "USBVision", sizeof(vc->driver));
	strlcpy(vc->card,
		usbvision_device_data[usbvision->dev_model].model_string,
		sizeof(vc->card));
	usb_make_path(usbvision->dev, vc->bus_info, sizeof(vc->bus_info));
	vc->device_caps = usbvision->have_tuner ? V4L2_CAP_TUNER : 0;
	if (vdev->vfl_type == VFL_TYPE_GRABBER)
		vc->device_caps |= V4L2_CAP_VIDEO_CAPTURE |
			V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
	else
		vc->device_caps |= V4L2_CAP_RADIO;

	vc->capabilities = vc->device_caps | V4L2_CAP_VIDEO_CAPTURE |
		V4L2_CAP_READWRITE | V4L2_CAP_STREAMING | V4L2_CAP_DEVICE_CAPS;
	if (usbvision_device_data[usbvision->dev_model].radio)
		vc->capabilities |= V4L2_CAP_RADIO;
	return 0;
}
static void usbvision_vdev_init(struct usb_usbvision *usbvision,
				struct video_device *vdev,
				const struct video_device *vdev_template,
				const char *name)
{
	struct usb_device *usb_dev = usbvision->dev;

	if (usb_dev == NULL) {
		dev_err(&usbvision->dev->dev,
			"%s: usbvision->dev is not set\n", __func__);
		return;
	}

	*vdev = *vdev_template;
	vdev->lock = &usbvision->v4l2_lock;
	vdev->v4l2_dev = &usbvision->v4l2_dev;
	snprintf(vdev->name, sizeof(vdev->name), "%s", name);
	video_set_drvdata(vdev, usbvision);
}
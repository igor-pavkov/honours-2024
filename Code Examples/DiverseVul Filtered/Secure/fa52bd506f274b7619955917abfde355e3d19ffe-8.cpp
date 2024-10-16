static void usbvision_create_sysfs(struct video_device *vdev)
{
	int res;

	if (!vdev)
		return;
	do {
		res = device_create_file(&vdev->dev, &dev_attr_version);
		if (res < 0)
			break;
		res = device_create_file(&vdev->dev, &dev_attr_model);
		if (res < 0)
			break;
		res = device_create_file(&vdev->dev, &dev_attr_hue);
		if (res < 0)
			break;
		res = device_create_file(&vdev->dev, &dev_attr_contrast);
		if (res < 0)
			break;
		res = device_create_file(&vdev->dev, &dev_attr_brightness);
		if (res < 0)
			break;
		res = device_create_file(&vdev->dev, &dev_attr_saturation);
		if (res < 0)
			break;
		res = device_create_file(&vdev->dev, &dev_attr_streaming);
		if (res < 0)
			break;
		res = device_create_file(&vdev->dev, &dev_attr_compression);
		if (res < 0)
			break;
		res = device_create_file(&vdev->dev, &dev_attr_bridge);
		if (res >= 0)
			return;
	} while (0);

	dev_err(&vdev->dev, "%s error: %d\n", __func__, res);
}
static void usbvision_remove_sysfs(struct video_device *vdev)
{
	if (vdev) {
		device_remove_file(&vdev->dev, &dev_attr_version);
		device_remove_file(&vdev->dev, &dev_attr_model);
		device_remove_file(&vdev->dev, &dev_attr_hue);
		device_remove_file(&vdev->dev, &dev_attr_contrast);
		device_remove_file(&vdev->dev, &dev_attr_brightness);
		device_remove_file(&vdev->dev, &dev_attr_saturation);
		device_remove_file(&vdev->dev, &dev_attr_streaming);
		device_remove_file(&vdev->dev, &dev_attr_compression);
		device_remove_file(&vdev->dev, &dev_attr_bridge);
	}
}
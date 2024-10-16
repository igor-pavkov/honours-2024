static ssize_t show_device_bridge(struct device *cd,
				  struct device_attribute *attr, char *buf)
{
	struct video_device *vdev =
		container_of(cd, struct video_device, dev);
	struct usb_usbvision *usbvision = video_get_drvdata(vdev);
	return sprintf(buf, "%d\n", usbvision->bridge_type);
}
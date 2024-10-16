static ssize_t show_model(struct device *cd,
			  struct device_attribute *attr, char *buf)
{
	struct video_device *vdev =
		container_of(cd, struct video_device, dev);
	struct usb_usbvision *usbvision = video_get_drvdata(vdev);
	return sprintf(buf, "%s\n",
		       usbvision_device_data[usbvision->dev_model].model_string);
}
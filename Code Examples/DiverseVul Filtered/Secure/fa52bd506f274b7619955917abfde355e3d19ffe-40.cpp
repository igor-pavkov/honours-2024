static inline struct usb_usbvision *cd_to_usbvision(struct device *cd)
{
	struct video_device *vdev =
		container_of(cd, struct video_device, dev);
	return video_get_drvdata(vdev);
}
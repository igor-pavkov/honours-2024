static int em28xx_media_device_init(struct em28xx *dev,
				    struct usb_device *udev)
{
#ifdef CONFIG_MEDIA_CONTROLLER
	struct media_device *mdev;

	mdev = kzalloc(sizeof(*mdev), GFP_KERNEL);
	if (!mdev)
		return -ENOMEM;

	if (udev->product)
		media_device_usb_init(mdev, udev, udev->product);
	else if (udev->manufacturer)
		media_device_usb_init(mdev, udev, udev->manufacturer);
	else
		media_device_usb_init(mdev, udev, dev_name(&dev->intf->dev));

	dev->media_dev = mdev;
#endif
	return 0;
}
static void *siano_media_device_register(struct smsusb_device_t *dev,
					int board_id)
{
#ifdef CONFIG_MEDIA_CONTROLLER_DVB
	struct media_device *mdev;
	struct usb_device *udev = dev->udev;
	struct sms_board *board = sms_get_board(board_id);
	int ret;

	mdev = kzalloc(sizeof(*mdev), GFP_KERNEL);
	if (!mdev)
		return NULL;

	media_device_usb_init(mdev, udev, board->name);

	ret = media_device_register(mdev);
	if (ret) {
		media_device_cleanup(mdev);
		kfree(mdev);
		return NULL;
	}

	pr_info("media controller created\n");

	return mdev;
#else
	return NULL;
#endif
}
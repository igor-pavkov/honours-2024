static void em28xx_unregister_media_device(struct em28xx *dev)
{
#ifdef CONFIG_MEDIA_CONTROLLER
	if (dev->media_dev) {
		media_device_unregister(dev->media_dev);
		media_device_cleanup(dev->media_dev);
		kfree(dev->media_dev);
		dev->media_dev = NULL;
	}
#endif
}
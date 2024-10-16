void em28xx_free_device(struct kref *ref)
{
	struct em28xx *dev = kref_to_dev(ref);

	dev_info(&dev->intf->dev, "Freeing device\n");

	if (!dev->disconnected)
		em28xx_release_resources(dev);

	if (dev->ts == PRIMARY_TS)
		kfree(dev->alt_max_pkt_size_isoc);

	kfree(dev);
}
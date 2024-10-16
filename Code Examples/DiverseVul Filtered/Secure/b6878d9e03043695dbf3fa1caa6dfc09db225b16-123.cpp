static int md_media_changed(struct gendisk *disk)
{
	struct mddev *mddev = disk->private_data;

	return mddev->changed;
}
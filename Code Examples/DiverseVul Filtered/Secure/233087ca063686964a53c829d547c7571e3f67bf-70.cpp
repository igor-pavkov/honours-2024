static int invalidate_drive(struct block_device *bdev)
{
	/* invalidate the buffer track to force a reread */
	set_bit((long)bdev->bd_disk->private_data, &fake_change);
	process_fd_request();
	if (bdev_check_media_change(bdev))
		floppy_revalidate(bdev->bd_disk);
	return 0;
}
static void floppy_release(struct gendisk *disk, fmode_t mode)
{
	int drive = (long)disk->private_data;

	mutex_lock(&floppy_mutex);
	mutex_lock(&open_lock);
	if (!drive_state[drive].fd_ref--) {
		DPRINT("floppy_release with fd_ref == 0");
		drive_state[drive].fd_ref = 0;
	}
	if (!drive_state[drive].fd_ref)
		opened_bdev[drive] = NULL;
	mutex_unlock(&open_lock);
	mutex_unlock(&floppy_mutex);
}
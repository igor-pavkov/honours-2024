static unsigned int floppy_check_events(struct gendisk *disk,
					unsigned int clearing)
{
	int drive = (long)disk->private_data;

	if (test_bit(FD_DISK_CHANGED_BIT, &drive_state[drive].flags) ||
	    test_bit(FD_VERIFY_BIT, &drive_state[drive].flags))
		return DISK_EVENT_MEDIA_CHANGE;

	if (time_after(jiffies, drive_state[drive].last_checked + drive_params[drive].checkfreq)) {
		if (lock_fdc(drive))
			return 0;
		poll_drive(false, 0);
		process_fd_request();
	}

	if (test_bit(FD_DISK_CHANGED_BIT, &drive_state[drive].flags) ||
	    test_bit(FD_VERIFY_BIT, &drive_state[drive].flags) ||
	    test_bit(drive, &fake_change) ||
	    drive_no_geom(drive))
		return DISK_EVENT_MEDIA_CHANGE;
	return 0;
}
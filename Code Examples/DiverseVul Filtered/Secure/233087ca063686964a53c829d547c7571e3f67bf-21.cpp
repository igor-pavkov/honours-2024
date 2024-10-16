static int floppy_open(struct block_device *bdev, fmode_t mode)
{
	int drive = (long)bdev->bd_disk->private_data;
	int old_dev, new_dev;
	int try;
	int res = -EBUSY;
	char *tmp;

	mutex_lock(&floppy_mutex);
	mutex_lock(&open_lock);
	old_dev = drive_state[drive].fd_device;
	if (opened_bdev[drive] && opened_bdev[drive] != bdev)
		goto out2;

	if (!drive_state[drive].fd_ref && (drive_params[drive].flags & FD_BROKEN_DCL)) {
		set_bit(FD_DISK_CHANGED_BIT, &drive_state[drive].flags);
		set_bit(FD_VERIFY_BIT, &drive_state[drive].flags);
	}

	drive_state[drive].fd_ref++;

	opened_bdev[drive] = bdev;

	res = -ENXIO;

	if (!floppy_track_buffer) {
		/* if opening an ED drive, reserve a big buffer,
		 * else reserve a small one */
		if ((drive_params[drive].cmos == 6) || (drive_params[drive].cmos == 5))
			try = 64;	/* Only 48 actually useful */
		else
			try = 32;	/* Only 24 actually useful */

		tmp = (char *)fd_dma_mem_alloc(1024 * try);
		if (!tmp && !floppy_track_buffer) {
			try >>= 1;	/* buffer only one side */
			INFBOUND(try, 16);
			tmp = (char *)fd_dma_mem_alloc(1024 * try);
		}
		if (!tmp && !floppy_track_buffer)
			fallback_on_nodma_alloc(&tmp, 2048 * try);
		if (!tmp && !floppy_track_buffer) {
			DPRINT("Unable to allocate DMA memory\n");
			goto out;
		}
		if (floppy_track_buffer) {
			if (tmp)
				fd_dma_mem_free((unsigned long)tmp, try * 1024);
		} else {
			buffer_min = buffer_max = -1;
			floppy_track_buffer = tmp;
			max_buffer_sectors = try;
		}
	}

	new_dev = MINOR(bdev->bd_dev);
	drive_state[drive].fd_device = new_dev;
	set_capacity(disks[drive][ITYPE(new_dev)], floppy_sizes[new_dev]);
	if (old_dev != -1 && old_dev != new_dev) {
		if (buffer_drive == drive)
			buffer_track = -1;
	}

	if (fdc_state[FDC(drive)].rawcmd == 1)
		fdc_state[FDC(drive)].rawcmd = 2;

	if (!(mode & FMODE_NDELAY)) {
		if (mode & (FMODE_READ|FMODE_WRITE)) {
			drive_state[drive].last_checked = 0;
			clear_bit(FD_OPEN_SHOULD_FAIL_BIT,
				  &drive_state[drive].flags);
			if (bdev_check_media_change(bdev))
				floppy_revalidate(bdev->bd_disk);
			if (test_bit(FD_DISK_CHANGED_BIT, &drive_state[drive].flags))
				goto out;
			if (test_bit(FD_OPEN_SHOULD_FAIL_BIT, &drive_state[drive].flags))
				goto out;
		}
		res = -EROFS;
		if ((mode & FMODE_WRITE) &&
		    !test_bit(FD_DISK_WRITABLE_BIT, &drive_state[drive].flags))
			goto out;
	}
	mutex_unlock(&open_lock);
	mutex_unlock(&floppy_mutex);
	return 0;
out:
	drive_state[drive].fd_ref--;

	if (!drive_state[drive].fd_ref)
		opened_bdev[drive] = NULL;
out2:
	mutex_unlock(&open_lock);
	mutex_unlock(&floppy_mutex);
	return res;
}
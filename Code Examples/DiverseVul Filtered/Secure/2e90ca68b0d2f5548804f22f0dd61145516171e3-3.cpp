static int compat_getdrvstat(int drive, bool poll,
			    struct compat_floppy_drive_struct __user *arg)
{
	struct compat_floppy_drive_struct v;

	memset(&v, 0, sizeof(struct compat_floppy_drive_struct));
	mutex_lock(&floppy_mutex);

	if (poll) {
		if (lock_fdc(drive))
			goto Eintr;
		if (poll_drive(true, FD_RAW_NEED_DISK) == -EINTR)
			goto Eintr;
		process_fd_request();
	}
	v.spinup_date = UDRS->spinup_date;
	v.select_date = UDRS->select_date;
	v.first_read_date = UDRS->first_read_date;
	v.probed_format = UDRS->probed_format;
	v.track = UDRS->track;
	v.maxblock = UDRS->maxblock;
	v.maxtrack = UDRS->maxtrack;
	v.generation = UDRS->generation;
	v.keep_data = UDRS->keep_data;
	v.fd_ref = UDRS->fd_ref;
	v.fd_device = UDRS->fd_device;
	v.last_checked = UDRS->last_checked;
	v.dmabuf = (uintptr_t)UDRS->dmabuf;
	v.bufblocks = UDRS->bufblocks;
	mutex_unlock(&floppy_mutex);

	if (copy_to_user(arg, &v, sizeof(struct compat_floppy_drive_struct)))
		return -EFAULT;
	return 0;
Eintr:
	mutex_unlock(&floppy_mutex);
	return -EINTR;
}
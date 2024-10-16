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
	v.spinup_date = drive_state[drive].spinup_date;
	v.select_date = drive_state[drive].select_date;
	v.first_read_date = drive_state[drive].first_read_date;
	v.probed_format = drive_state[drive].probed_format;
	v.track = drive_state[drive].track;
	v.maxblock = drive_state[drive].maxblock;
	v.maxtrack = drive_state[drive].maxtrack;
	v.generation = drive_state[drive].generation;
	v.keep_data = drive_state[drive].keep_data;
	v.fd_ref = drive_state[drive].fd_ref;
	v.fd_device = drive_state[drive].fd_device;
	v.last_checked = drive_state[drive].last_checked;
	v.dmabuf = (uintptr_t) drive_state[drive].dmabuf;
	v.bufblocks = drive_state[drive].bufblocks;
	mutex_unlock(&floppy_mutex);

	if (copy_to_user(arg, &v, sizeof(struct compat_floppy_drive_struct)))
		return -EFAULT;
	return 0;
Eintr:
	mutex_unlock(&floppy_mutex);
	return -EINTR;
}
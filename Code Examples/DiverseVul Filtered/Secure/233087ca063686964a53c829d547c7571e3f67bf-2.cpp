static int compat_get_prm(int drive,
			  struct compat_floppy_struct __user *arg)
{
	struct compat_floppy_struct v;
	struct floppy_struct *p;
	int err;

	memset(&v, 0, sizeof(v));
	mutex_lock(&floppy_mutex);
	err = get_floppy_geometry(drive, ITYPE(drive_state[drive].fd_device),
				  &p);
	if (err) {
		mutex_unlock(&floppy_mutex);
		return err;
	}
	memcpy(&v, p, offsetof(struct floppy_struct, name));
	mutex_unlock(&floppy_mutex);
	if (copy_to_user(arg, &v, sizeof(struct compat_floppy_struct)))
		return -EFAULT;
	return 0;
}
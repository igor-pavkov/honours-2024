static int compat_set_geometry(struct block_device *bdev, fmode_t mode, unsigned int cmd,
		    struct compat_floppy_struct __user *arg)
{
	struct floppy_struct v;
	int drive, type;
	int err;

	BUILD_BUG_ON(offsetof(struct floppy_struct, name) !=
		     offsetof(struct compat_floppy_struct, name));

	if (!(mode & (FMODE_WRITE | FMODE_WRITE_IOCTL)))
		return -EPERM;

	memset(&v, 0, sizeof(struct floppy_struct));
	if (copy_from_user(&v, arg, offsetof(struct floppy_struct, name)))
		return -EFAULT;

	mutex_lock(&floppy_mutex);
	drive = (long)bdev->bd_disk->private_data;
	type = ITYPE(drive_state[drive].fd_device);
	err = set_geometry(cmd == FDSETPRM32 ? FDSETPRM : FDDEFPRM,
			&v, drive, type, bdev);
	mutex_unlock(&floppy_mutex);
	return err;
}
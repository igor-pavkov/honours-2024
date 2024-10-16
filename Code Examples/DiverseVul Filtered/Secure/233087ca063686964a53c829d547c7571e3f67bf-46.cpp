static int compat_werrorget(int drive,
			    struct compat_floppy_write_errors __user *arg)
{
	struct compat_floppy_write_errors v32;
	struct floppy_write_errors v;

	memset(&v32, 0, sizeof(struct compat_floppy_write_errors));
	mutex_lock(&floppy_mutex);
	v = write_errors[drive];
	mutex_unlock(&floppy_mutex);
	v32.write_errors = v.write_errors;
	v32.first_error_sector = v.first_error_sector;
	v32.first_error_generation = v.first_error_generation;
	v32.last_error_sector = v.last_error_sector;
	v32.last_error_generation = v.last_error_generation;
	v32.badness = v.badness;
	if (copy_to_user(arg, &v32, sizeof(struct compat_floppy_write_errors)))
		return -EFAULT;
	return 0;
}
static long dm_compat_ctl_ioctl(struct file *file, uint command, ulong u)
{
	return (long)dm_ctl_ioctl(file, command, (ulong) compat_ptr(u));
}
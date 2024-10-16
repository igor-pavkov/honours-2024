static long dm_ctl_ioctl(struct file *file, uint command, ulong u)
{
	return (long)ctl_ioctl(file, command, (struct dm_ioctl __user *)u);
}
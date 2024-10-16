static int inotify_fasync(int fd, struct file *file, int on)
{
	struct inotify_device *dev = file->private_data;

	return fasync_helper(fd, file, on, &dev->fa) >= 0 ? 0 : -EIO;
}
static int floppy_raw_cmd_ioctl(int type, int drive, int cmd,
				void __user *param)
{
	int ret;

	pr_warn_once("Note: FDRAWCMD is deprecated and will be removed from the kernel in the near future.\n");

	if (type)
		return -EINVAL;
	if (lock_fdc(drive))
		return -EINTR;
	set_floppy(drive);
	ret = raw_cmd_ioctl(cmd, param);
	if (ret == -EINTR)
		return -EINTR;
	process_fd_request();
	return ret;
}
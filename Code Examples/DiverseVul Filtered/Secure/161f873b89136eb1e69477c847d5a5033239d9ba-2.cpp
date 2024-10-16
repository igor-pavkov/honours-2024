SYSCALL_DEFINE3(open_by_handle_at, int, mountdirfd,
		struct file_handle __user *, handle,
		int, flags)
{
	long ret;

	if (force_o_largefile())
		flags |= O_LARGEFILE;

	ret = do_handle_open(mountdirfd, handle, flags);
	return ret;
}
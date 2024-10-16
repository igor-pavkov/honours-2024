asmlinkage long sys_openat(int dfd, const char __user *filename, int flags,
			   int mode)
{
	long ret;

	if (force_o_largefile())
		flags |= O_LARGEFILE;

	ret = do_sys_open(dfd, filename, flags, mode);
	/* avoid REGPARM breakage on x86: */
	prevent_tail_call(ret);
	return ret;
}
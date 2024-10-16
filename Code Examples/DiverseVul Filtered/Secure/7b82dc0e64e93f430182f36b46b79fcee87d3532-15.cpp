asmlinkage long sys_ftruncate64(unsigned int fd, loff_t length)
{
	long ret = do_sys_ftruncate(fd, length, 0);
	/* avoid REGPARM breakage on x86: */
	prevent_tail_call(ret);
	return ret;
}
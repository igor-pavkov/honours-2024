asmlinkage long sys_truncate(const char __user * path, unsigned long length)
{
	/* on 32-bit boxen it will cut the range 2^31--2^32-1 off */
	return do_sys_truncate(path, (long)length);
}
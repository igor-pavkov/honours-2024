asmlinkage long sys_truncate64(const char __user * path, loff_t length)
{
	return do_sys_truncate(path, length);
}
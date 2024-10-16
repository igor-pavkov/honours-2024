asmlinkage long sys_chmod(const char __user *filename, mode_t mode)
{
	return sys_fchmodat(AT_FDCWD, filename, mode);
}
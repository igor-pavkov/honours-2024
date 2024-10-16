asmlinkage long sys_oldumount(char __user * name)
{
	return sys_umount(name, 0);
}
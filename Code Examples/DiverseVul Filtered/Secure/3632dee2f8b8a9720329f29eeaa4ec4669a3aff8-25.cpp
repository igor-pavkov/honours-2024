SYSCALL_DEFINE0(inotify_init)
{
	return sys_inotify_init1(0);
}
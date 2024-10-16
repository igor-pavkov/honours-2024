static int selinux_task_setnice(struct task_struct *p, int nice)
{
	return current_has_perm(p, PROCESS__SETSCHED);
}
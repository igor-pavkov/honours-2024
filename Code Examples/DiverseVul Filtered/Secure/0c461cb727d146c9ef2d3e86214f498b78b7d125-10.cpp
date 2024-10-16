static int selinux_task_setscheduler(struct task_struct *p)
{
	return current_has_perm(p, PROCESS__SETSCHED);
}
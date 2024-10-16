static int selinux_task_setioprio(struct task_struct *p, int ioprio)
{
	return current_has_perm(p, PROCESS__SETSCHED);
}
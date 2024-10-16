static int selinux_ptrace_traceme(struct task_struct *parent)
{
	return task_has_perm(parent, current, PROCESS__PTRACE);
}
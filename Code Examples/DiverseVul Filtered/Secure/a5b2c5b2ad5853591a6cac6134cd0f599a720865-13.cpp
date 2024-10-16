static int apparmor_ptrace_access_check(struct task_struct *child,
					unsigned int mode)
{
	int error = cap_ptrace_access_check(child, mode);
	if (error)
		return error;

	return aa_ptrace(current, child, mode);
}
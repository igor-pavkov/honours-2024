static int selinux_ptrace_access_check(struct task_struct *child,
				     unsigned int mode)
{
	if (mode & PTRACE_MODE_READ) {
		u32 sid = current_sid();
		u32 csid = task_sid(child);
		return avc_has_perm(sid, csid, SECCLASS_FILE, FILE__READ, NULL);
	}

	return current_has_perm(child, PROCESS__PTRACE);
}
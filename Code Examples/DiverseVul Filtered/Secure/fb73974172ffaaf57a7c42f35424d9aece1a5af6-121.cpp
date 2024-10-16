static int selinux_task_alloc(struct task_struct *task,
			      unsigned long clone_flags)
{
	u32 sid = current_sid();

	return avc_has_perm(&selinux_state,
			    sid, sid, SECCLASS_PROCESS, PROCESS__FORK, NULL);
}
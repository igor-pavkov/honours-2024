int task_current_syscall(struct task_struct *target, struct syscall_info *info)
{
	long state;
	unsigned long ncsw;

	if (target == current)
		return collect_syscall(target, info);

	state = target->state;
	if (unlikely(!state))
		return -EAGAIN;

	ncsw = wait_task_inactive(target, state);
	if (unlikely(!ncsw) ||
	    unlikely(collect_syscall(target, info)) ||
	    unlikely(wait_task_inactive(target, state) != ncsw))
		return -EAGAIN;

	return 0;
}
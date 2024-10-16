static int control_ops_alloc(struct ftrace_ops *ops)
{
	int __percpu *disabled;

	disabled = alloc_percpu(int);
	if (!disabled)
		return -ENOMEM;

	ops->disabled = disabled;
	control_ops_disable_all(ops);
	return 0;
}
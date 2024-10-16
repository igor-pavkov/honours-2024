static void control_ops_free(struct ftrace_ops *ops)
{
	free_percpu(ops->disabled);
}
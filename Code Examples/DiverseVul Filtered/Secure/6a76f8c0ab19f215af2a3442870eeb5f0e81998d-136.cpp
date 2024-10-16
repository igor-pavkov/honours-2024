static void add_ftrace_ops(struct ftrace_ops **list, struct ftrace_ops *ops)
{
	ops->next = *list;
	/*
	 * We are entering ops into the list but another
	 * CPU might be walking that list. We need to make sure
	 * the ops->next pointer is valid before another CPU sees
	 * the ops pointer included into the list.
	 */
	rcu_assign_pointer(*list, ops);
}
static void ftrace_free_entry_rcu(struct rcu_head *rhp)
{
	struct ftrace_func_probe *entry =
		container_of(rhp, struct ftrace_func_probe, rcu);

	if (entry->ops->free)
		entry->ops->free(&entry->data);
	kfree(entry);
}
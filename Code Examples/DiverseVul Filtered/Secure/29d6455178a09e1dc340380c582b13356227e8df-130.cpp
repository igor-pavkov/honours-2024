static void sched_free_group_rcu(struct rcu_head *rhp)
{
	/* now it should be safe to free those cfs_rqs */
	sched_free_group(container_of(rhp, struct task_group, rcu));
}
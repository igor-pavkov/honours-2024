static void delayed_free_vfsmnt(struct rcu_head *head)
{
	free_vfsmnt(container_of(head, struct mount, mnt_rcu));
}
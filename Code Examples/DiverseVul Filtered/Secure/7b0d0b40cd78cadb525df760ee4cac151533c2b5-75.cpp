static void inode_free_rcu(struct rcu_head *head)
{
	struct inode_security_struct *isec;

	isec = container_of(head, struct inode_security_struct, rcu);
	kmem_cache_free(sel_inode_cache, isec);
}
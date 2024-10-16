void proc_sys_evict_inode(struct inode *inode, struct ctl_table_header *head)
{
	spin_lock(&sysctl_lock);
	hlist_del_init_rcu(&PROC_I(inode)->sysctl_inodes);
	if (!--head->count)
		kfree_rcu(head, rcu);
	spin_unlock(&sysctl_lock);
}
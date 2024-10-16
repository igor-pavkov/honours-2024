static void nfs_access_free_entry(struct nfs_access_entry *entry)
{
	put_group_info(entry->group_info);
	kfree_rcu(entry, rcu_head);
	smp_mb__before_atomic();
	atomic_long_dec(&nfs_access_nr_entries);
	smp_mb__after_atomic();
}
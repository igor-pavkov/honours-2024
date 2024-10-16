static bool is_mnt_ns_file(struct dentry *dentry)
{
	/* Is this a proxy for a mount namespace? */
	struct inode *inode = dentry->d_inode;
	struct proc_ns *ei;

	if (!proc_ns_inode(inode))
		return false;

	ei = get_proc_ns(inode);
	if (ei->ns_ops != &mntns_operations)
		return false;

	return true;
}
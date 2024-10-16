void nfs_clear_verifier_delegated(struct inode *inode)
{
	struct dentry *alias;

	if (!inode)
		return;
	spin_lock(&inode->i_lock);
	hlist_for_each_entry(alias, &inode->i_dentry, d_u.d_alias) {
		spin_lock(&alias->d_lock);
		nfs_unset_verifier_delegated(&alias->d_time);
		spin_unlock(&alias->d_lock);
	}
	spin_unlock(&inode->i_lock);
}
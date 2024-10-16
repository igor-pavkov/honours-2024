void dnotify_parent(struct dentry *dentry, unsigned long event)
{
	struct dentry *parent;

	if (!dir_notify_enable)
		return;

	spin_lock(&dentry->d_lock);
	parent = dentry->d_parent;
	if (parent->d_inode->i_dnotify_mask & event) {
		dget(parent);
		spin_unlock(&dentry->d_lock);
		__inode_dir_notify(parent->d_inode, event);
		dput(parent);
	} else {
		spin_unlock(&dentry->d_lock);
	}
}
void dnotify_flush(struct file *filp, fl_owner_t id)
{
	struct dnotify_struct *dn;
	struct dnotify_struct **prev;
	struct inode *inode;

	inode = filp->f_path.dentry->d_inode;
	if (!S_ISDIR(inode->i_mode))
		return;
	spin_lock(&inode->i_lock);
	prev = &inode->i_dnotify;
	while ((dn = *prev) != NULL) {
		if ((dn->dn_owner == id) && (dn->dn_filp == filp)) {
			*prev = dn->dn_next;
			redo_inode_mask(inode);
			kmem_cache_free(dn_cache, dn);
			break;
		}
		prev = &dn->dn_next;
	}
	spin_unlock(&inode->i_lock);
}
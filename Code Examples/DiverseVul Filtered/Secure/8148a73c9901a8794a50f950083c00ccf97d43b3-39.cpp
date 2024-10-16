int proc_setattr(struct dentry *dentry, struct iattr *attr)
{
	int error;
	struct inode *inode = d_inode(dentry);

	if (attr->ia_valid & ATTR_MODE)
		return -EPERM;

	error = inode_change_ok(inode, attr);
	if (error)
		return error;

	setattr_copy(inode, attr);
	mark_inode_dirty(inode);
	return 0;
}
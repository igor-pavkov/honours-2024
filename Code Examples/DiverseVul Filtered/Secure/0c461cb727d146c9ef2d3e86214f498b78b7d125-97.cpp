static void selinux_inode_invalidate_secctx(struct inode *inode)
{
	struct inode_security_struct *isec = inode->i_security;

	spin_lock(&isec->lock);
	isec->initialized = LABEL_INVALID;
	spin_unlock(&isec->lock);
}
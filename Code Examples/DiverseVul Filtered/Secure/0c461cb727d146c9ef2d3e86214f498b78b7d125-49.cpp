static int __inode_security_revalidate(struct inode *inode,
				       struct dentry *opt_dentry,
				       bool may_sleep)
{
	struct inode_security_struct *isec = inode->i_security;

	might_sleep_if(may_sleep);

	if (ss_initialized && isec->initialized != LABEL_INITIALIZED) {
		if (!may_sleep)
			return -ECHILD;

		/*
		 * Try reloading the inode security label.  This will fail if
		 * @opt_dentry is NULL and no dentry for this inode can be
		 * found; in that case, continue using the old label.
		 */
		inode_doinit_with_dentry(inode, opt_dentry);
	}
	return 0;
}
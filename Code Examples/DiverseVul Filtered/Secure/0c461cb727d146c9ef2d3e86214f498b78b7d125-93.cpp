static int selinux_file_permission(struct file *file, int mask)
{
	struct inode *inode = file_inode(file);
	struct file_security_struct *fsec = file->f_security;
	struct inode_security_struct *isec;
	u32 sid = current_sid();

	if (!mask)
		/* No permission to check.  Existence test. */
		return 0;

	isec = inode_security(inode);
	if (sid == fsec->sid && fsec->isid == isec->sid &&
	    fsec->pseqno == avc_policy_seqno())
		/* No change since file_open check. */
		return 0;

	return selinux_revalidate_file_permission(file, mask);
}
static int selinux_inode_notifysecctx(struct inode *inode, void *ctx, u32 ctxlen)
{
	int rc = selinux_inode_setsecurity(inode, XATTR_SELINUX_SUFFIX,
					   ctx, ctxlen, 0);
	/* Do not return error when suppressing label (SBLABEL_MNT not set). */
	return rc == -EOPNOTSUPP ? 0 : rc;
}
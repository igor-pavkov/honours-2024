static int selinux_dentry_init_security(struct dentry *dentry, int mode,
					const struct qstr *name, void **ctx,
					u32 *ctxlen)
{
	u32 newsid;
	int rc;

	rc = selinux_determine_inode_label(current_security(),
					   d_inode(dentry->d_parent), name,
					   inode_mode_to_security_class(mode),
					   &newsid);
	if (rc)
		return rc;

	return security_sid_to_context(newsid, (char **)ctx, ctxlen);
}
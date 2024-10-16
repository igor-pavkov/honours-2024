static int selinux_dentry_init_security(struct dentry *dentry, int mode,
					const struct qstr *name, void **ctx,
					u32 *ctxlen)
{
	u32 newsid;
	int rc;

	rc = selinux_determine_inode_label(selinux_cred(current_cred()),
					   d_inode(dentry->d_parent), name,
					   inode_mode_to_security_class(mode),
					   &newsid);
	if (rc)
		return rc;

	return security_sid_to_context(&selinux_state, newsid, (char **)ctx,
				       ctxlen);
}
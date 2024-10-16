static int selinux_dentry_init_security(struct dentry *dentry, int mode,
					struct qstr *name, void **ctx,
					u32 *ctxlen)
{
	const struct cred *cred = current_cred();
	struct task_security_struct *tsec;
	struct inode_security_struct *dsec;
	struct superblock_security_struct *sbsec;
	struct inode *dir = dentry->d_parent->d_inode;
	u32 newsid;
	int rc;

	tsec = cred->security;
	dsec = dir->i_security;
	sbsec = dir->i_sb->s_security;

	if (tsec->create_sid && sbsec->behavior != SECURITY_FS_USE_MNTPOINT) {
		newsid = tsec->create_sid;
	} else {
		rc = security_transition_sid(tsec->sid, dsec->sid,
					     inode_mode_to_security_class(mode),
					     name,
					     &newsid);
		if (rc) {
			printk(KERN_WARNING
				"%s: security_transition_sid failed, rc=%d\n",
			       __func__, -rc);
			return rc;
		}
	}

	return security_sid_to_context(newsid, (char **)ctx, ctxlen);
}
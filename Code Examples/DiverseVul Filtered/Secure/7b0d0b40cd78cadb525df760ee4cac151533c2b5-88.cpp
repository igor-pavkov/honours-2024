static int selinux_inode_init_security(struct inode *inode, struct inode *dir,
				       const struct qstr *qstr,
				       const char **name,
				       void **value, size_t *len)
{
	const struct task_security_struct *tsec = current_security();
	struct inode_security_struct *dsec;
	struct superblock_security_struct *sbsec;
	u32 sid, newsid, clen;
	int rc;
	char *context;

	dsec = dir->i_security;
	sbsec = dir->i_sb->s_security;

	sid = tsec->sid;
	newsid = tsec->create_sid;

	if ((sbsec->flags & SE_SBINITIALIZED) &&
	    (sbsec->behavior == SECURITY_FS_USE_MNTPOINT))
		newsid = sbsec->mntpoint_sid;
	else if (!newsid || !(sbsec->flags & SBLABEL_MNT)) {
		rc = security_transition_sid(sid, dsec->sid,
					     inode_mode_to_security_class(inode->i_mode),
					     qstr, &newsid);
		if (rc) {
			printk(KERN_WARNING "%s:  "
			       "security_transition_sid failed, rc=%d (dev=%s "
			       "ino=%ld)\n",
			       __func__,
			       -rc, inode->i_sb->s_id, inode->i_ino);
			return rc;
		}
	}

	/* Possibly defer initialization to selinux_complete_init. */
	if (sbsec->flags & SE_SBINITIALIZED) {
		struct inode_security_struct *isec = inode->i_security;
		isec->sclass = inode_mode_to_security_class(inode->i_mode);
		isec->sid = newsid;
		isec->initialized = 1;
	}

	if (!ss_initialized || !(sbsec->flags & SBLABEL_MNT))
		return -EOPNOTSUPP;

	if (name)
		*name = XATTR_SELINUX_SUFFIX;

	if (value && len) {
		rc = security_sid_to_context_force(newsid, &context, &clen);
		if (rc)
			return rc;
		*value = context;
		*len = clen;
	}

	return 0;
}
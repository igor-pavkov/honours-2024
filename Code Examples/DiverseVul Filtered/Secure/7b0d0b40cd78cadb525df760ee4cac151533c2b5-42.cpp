static int may_create(struct inode *dir,
		      struct dentry *dentry,
		      u16 tclass)
{
	const struct task_security_struct *tsec = current_security();
	struct inode_security_struct *dsec;
	struct superblock_security_struct *sbsec;
	u32 sid, newsid;
	struct common_audit_data ad;
	int rc;

	dsec = dir->i_security;
	sbsec = dir->i_sb->s_security;

	sid = tsec->sid;
	newsid = tsec->create_sid;

	ad.type = LSM_AUDIT_DATA_DENTRY;
	ad.u.dentry = dentry;

	rc = avc_has_perm(sid, dsec->sid, SECCLASS_DIR,
			  DIR__ADD_NAME | DIR__SEARCH,
			  &ad);
	if (rc)
		return rc;

	if (!newsid || !(sbsec->flags & SBLABEL_MNT)) {
		rc = security_transition_sid(sid, dsec->sid, tclass,
					     &dentry->d_name, &newsid);
		if (rc)
			return rc;
	}

	rc = avc_has_perm(sid, newsid, tclass, FILE__CREATE, &ad);
	if (rc)
		return rc;

	return avc_has_perm(newsid, sbsec->sid,
			    SECCLASS_FILESYSTEM,
			    FILESYSTEM__ASSOCIATE, &ad);
}
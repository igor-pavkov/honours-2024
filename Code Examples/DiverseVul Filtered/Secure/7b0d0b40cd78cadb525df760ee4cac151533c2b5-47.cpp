static int inode_has_perm(const struct cred *cred,
			  struct inode *inode,
			  u32 perms,
			  struct common_audit_data *adp)
{
	struct inode_security_struct *isec;
	u32 sid;

	validate_creds(cred);

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	sid = cred_sid(cred);
	isec = inode->i_security;

	return avc_has_perm(sid, isec->sid, isec->sclass, perms, adp);
}
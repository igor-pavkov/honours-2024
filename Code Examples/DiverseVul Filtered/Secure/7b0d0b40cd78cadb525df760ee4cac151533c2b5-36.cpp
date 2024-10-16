static noinline int audit_inode_permission(struct inode *inode,
					   u32 perms, u32 audited, u32 denied,
					   int result,
					   unsigned flags)
{
	struct common_audit_data ad;
	struct inode_security_struct *isec = inode->i_security;
	int rc;

	ad.type = LSM_AUDIT_DATA_INODE;
	ad.u.inode = inode;

	rc = slow_avc_audit(current_sid(), isec->sid, isec->sclass, perms,
			    audited, denied, result, &ad, flags);
	if (rc)
		return rc;
	return 0;
}
static int selinux_inode_setxattr(struct dentry *dentry, const char *name,
				  const void *value, size_t size, int flags)
{
	struct inode *inode = d_backing_inode(dentry);
	struct inode_security_struct *isec;
	struct superblock_security_struct *sbsec;
	struct common_audit_data ad;
	u32 newsid, sid = current_sid();
	int rc = 0;

	if (strcmp(name, XATTR_NAME_SELINUX))
		return selinux_inode_setotherxattr(dentry, name);

	sbsec = inode->i_sb->s_security;
	if (!(sbsec->flags & SBLABEL_MNT))
		return -EOPNOTSUPP;

	if (!inode_owner_or_capable(inode))
		return -EPERM;

	ad.type = LSM_AUDIT_DATA_DENTRY;
	ad.u.dentry = dentry;

	isec = backing_inode_security(dentry);
	rc = avc_has_perm(sid, isec->sid, isec->sclass,
			  FILE__RELABELFROM, &ad);
	if (rc)
		return rc;

	rc = security_context_to_sid(value, size, &newsid, GFP_KERNEL);
	if (rc == -EINVAL) {
		if (!capable(CAP_MAC_ADMIN)) {
			struct audit_buffer *ab;
			size_t audit_size;
			const char *str;

			/* We strip a nul only if it is at the end, otherwise the
			 * context contains a nul and we should audit that */
			if (value) {
				str = value;
				if (str[size - 1] == '\0')
					audit_size = size - 1;
				else
					audit_size = size;
			} else {
				str = "";
				audit_size = 0;
			}
			ab = audit_log_start(current->audit_context, GFP_ATOMIC, AUDIT_SELINUX_ERR);
			audit_log_format(ab, "op=setxattr invalid_context=");
			audit_log_n_untrustedstring(ab, value, audit_size);
			audit_log_end(ab);

			return rc;
		}
		rc = security_context_to_sid_force(value, size, &newsid);
	}
	if (rc)
		return rc;

	rc = avc_has_perm(sid, newsid, isec->sclass,
			  FILE__RELABELTO, &ad);
	if (rc)
		return rc;

	rc = security_validate_transition(isec->sid, newsid, sid,
					  isec->sclass);
	if (rc)
		return rc;

	return avc_has_perm(newsid,
			    sbsec->sid,
			    SECCLASS_FILESYSTEM,
			    FILESYSTEM__ASSOCIATE,
			    &ad);
}
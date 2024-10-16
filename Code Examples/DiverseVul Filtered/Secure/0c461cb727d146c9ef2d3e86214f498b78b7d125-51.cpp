static int selinux_inode_getsecurity(struct inode *inode, const char *name, void **buffer, bool alloc)
{
	u32 size;
	int error;
	char *context = NULL;
	struct inode_security_struct *isec;

	if (strcmp(name, XATTR_SELINUX_SUFFIX))
		return -EOPNOTSUPP;

	/*
	 * If the caller has CAP_MAC_ADMIN, then get the raw context
	 * value even if it is not defined by current policy; otherwise,
	 * use the in-core value under current policy.
	 * Use the non-auditing forms of the permission checks since
	 * getxattr may be called by unprivileged processes commonly
	 * and lack of permission just means that we fall back to the
	 * in-core context value, not a denial.
	 */
	error = cap_capable(current_cred(), &init_user_ns, CAP_MAC_ADMIN,
			    SECURITY_CAP_NOAUDIT);
	if (!error)
		error = cred_has_capability(current_cred(), CAP_MAC_ADMIN,
					    SECURITY_CAP_NOAUDIT, true);
	isec = inode_security(inode);
	if (!error)
		error = security_sid_to_context_force(isec->sid, &context,
						      &size);
	else
		error = security_sid_to_context(isec->sid, &context, &size);
	if (error)
		return error;
	error = size;
	if (alloc) {
		*buffer = context;
		goto out_nofree;
	}
	kfree(context);
out_nofree:
	return error;
}
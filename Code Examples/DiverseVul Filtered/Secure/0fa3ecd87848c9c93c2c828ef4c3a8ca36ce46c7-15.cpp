bool inode_owner_or_capable(const struct inode *inode)
{
	struct user_namespace *ns;

	if (uid_eq(current_fsuid(), inode->i_uid))
		return true;

	ns = current_user_ns();
	if (kuid_has_mapping(ns, inode->i_uid) && ns_capable(ns, CAP_FOWNER))
		return true;
	return false;
}
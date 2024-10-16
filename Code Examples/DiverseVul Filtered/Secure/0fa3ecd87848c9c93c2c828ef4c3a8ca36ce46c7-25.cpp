static int __remove_privs(struct dentry *dentry, int kill)
{
	struct iattr newattrs;

	newattrs.ia_valid = ATTR_FORCE | kill;
	/*
	 * Note we call this on write, so notify_change will not
	 * encounter any conflicting delegations:
	 */
	return notify_change(dentry, &newattrs, NULL);
}
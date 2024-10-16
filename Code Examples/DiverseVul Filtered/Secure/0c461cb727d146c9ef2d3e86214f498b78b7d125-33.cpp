static int selinux_inode_copy_up_xattr(const char *name)
{
	/* The copy_up hook above sets the initial context on an inode, but we
	 * don't then want to overwrite it by blindly copying all the lower
	 * xattrs up.  Instead, we have to filter out SELinux-related xattrs.
	 */
	if (strcmp(name, XATTR_NAME_SELINUX) == 0)
		return 1; /* Discard */
	/*
	 * Any other attribute apart from SELINUX is not claimed, supported
	 * by selinux.
	 */
	return -EOPNOTSUPP;
}
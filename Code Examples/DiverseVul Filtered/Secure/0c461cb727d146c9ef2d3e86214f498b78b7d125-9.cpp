static int selinux_inode_getattr(const struct path *path)
{
	return path_has_perm(current_cred(), path, FILE__GETATTR);
}
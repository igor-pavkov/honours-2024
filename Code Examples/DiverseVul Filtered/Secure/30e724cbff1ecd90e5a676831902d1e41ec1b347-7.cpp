static int vfswrap_removexattr(struct vfs_handle_struct *handle, const char *path, const char *name)
{
	return removexattr(path, name);
}
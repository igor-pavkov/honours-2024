static int vfswrap_setxattr(struct vfs_handle_struct *handle, const char *path, const char *name, const void *value, size_t size, int flags)
{
	return setxattr(path, name, value, size, flags);
}
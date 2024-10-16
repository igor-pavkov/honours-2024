static ssize_t vfswrap_getxattr(struct vfs_handle_struct *handle,const char *path, const char *name, void *value, size_t size)
{
	return getxattr(path, name, value, size);
}
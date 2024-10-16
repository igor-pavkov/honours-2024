static ssize_t vfswrap_listxattr(struct vfs_handle_struct *handle, const char *path, char *list, size_t size)
{
	return listxattr(path, list, size);
}
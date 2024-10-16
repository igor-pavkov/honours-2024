static ssize_t vfswrap_fgetxattr(struct vfs_handle_struct *handle, struct files_struct *fsp, const char *name, void *value, size_t size)
{
	return fgetxattr(fsp->fh->fd, name, value, size);
}
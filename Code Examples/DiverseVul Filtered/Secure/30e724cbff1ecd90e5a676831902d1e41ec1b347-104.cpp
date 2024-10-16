static ssize_t vfswrap_flistxattr(struct vfs_handle_struct *handle, struct files_struct *fsp, char *list, size_t size)
{
	return flistxattr(fsp->fh->fd, list, size);
}
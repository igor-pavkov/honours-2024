static int vfswrap_fsetxattr(struct vfs_handle_struct *handle, struct files_struct *fsp, const char *name, const void *value, size_t size, int flags)
{
	return fsetxattr(fsp->fh->fd, name, value, size, flags);
}
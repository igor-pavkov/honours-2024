static int vfswrap_fremovexattr(struct vfs_handle_struct *handle, struct files_struct *fsp, const char *name)
{
	return fremovexattr(fsp->fh->fd, name);
}
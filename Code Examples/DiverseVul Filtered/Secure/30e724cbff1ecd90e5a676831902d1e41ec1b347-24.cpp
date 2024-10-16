static int vfswrap_statvfs(struct vfs_handle_struct *handle, const char *path, vfs_statvfs_struct *statbuf)
{
	return sys_statvfs(path, statbuf);
}
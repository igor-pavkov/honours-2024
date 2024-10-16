static int vfswrap_fallocate(vfs_handle_struct *handle,
			files_struct *fsp,
			enum vfs_fallocate_mode mode,
			off_t offset,
			off_t len)
{
	int result;

	START_PROFILE(syscall_fallocate);
	if (mode == VFS_FALLOCATE_EXTEND_SIZE) {
		result = sys_posix_fallocate(fsp->fh->fd, offset, len);
	} else if (mode == VFS_FALLOCATE_KEEP_SIZE) {
		result = sys_fallocate(fsp->fh->fd, mode, offset, len);
	} else {
		errno = EINVAL;
		result = -1;
	}
	END_PROFILE(syscall_fallocate);
	return result;
}
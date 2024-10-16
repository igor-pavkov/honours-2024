static ssize_t vfswrap_recvfile(vfs_handle_struct *handle,
			int fromfd,
			files_struct *tofsp,
			off_t offset,
			size_t n)
{
	ssize_t result;

	START_PROFILE_BYTES(syscall_recvfile, n);
	result = sys_recvfile(fromfd, tofsp->fh->fd, offset, n);
	END_PROFILE(syscall_recvfile);
	return result;
}
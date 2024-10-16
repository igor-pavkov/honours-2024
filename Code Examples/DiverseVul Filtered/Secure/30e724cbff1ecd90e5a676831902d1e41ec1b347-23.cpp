static ssize_t vfswrap_sendfile(vfs_handle_struct *handle, int tofd, files_struct *fromfsp, const DATA_BLOB *hdr,
			off_t offset, size_t n)
{
	ssize_t result;

	START_PROFILE_BYTES(syscall_sendfile, n);
	result = sys_sendfile(tofd, fromfsp->fh->fd, hdr, offset, n);
	END_PROFILE(syscall_sendfile);
	return result;
}
static ssize_t vfswrap_pwrite(vfs_handle_struct *handle, files_struct *fsp, const void *data,
			size_t n, off_t offset)
{
	ssize_t result;

#if defined(HAVE_PWRITE) || defined(HAVE_PRWITE64)
	START_PROFILE_BYTES(syscall_pwrite, n);
	result = sys_pwrite(fsp->fh->fd, data, n, offset);
	END_PROFILE(syscall_pwrite);

	if (result == -1 && errno == ESPIPE) {
		/* Maintain the fiction that pipes can be sought on. */
		result = SMB_VFS_WRITE(fsp, data, n);
	}

#else /* HAVE_PWRITE */
	off_t   curr;
	int         lerrno;

	curr = SMB_VFS_LSEEK(fsp, 0, SEEK_CUR);
	if (curr == -1) {
		return -1;
	}

	if (SMB_VFS_LSEEK(fsp, offset, SEEK_SET) == -1) {
		return -1;
	}

	result = SMB_VFS_WRITE(fsp, data, n);
	lerrno = errno;

	SMB_VFS_LSEEK(fsp, curr, SEEK_SET);
	errno = lerrno;

#endif /* HAVE_PWRITE */

	return result;
}
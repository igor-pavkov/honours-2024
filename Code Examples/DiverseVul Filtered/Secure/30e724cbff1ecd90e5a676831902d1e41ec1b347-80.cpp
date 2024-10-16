static ssize_t vfswrap_pread(vfs_handle_struct *handle, files_struct *fsp, void *data,
			size_t n, off_t offset)
{
	ssize_t result;

#if defined(HAVE_PREAD) || defined(HAVE_PREAD64)
	START_PROFILE_BYTES(syscall_pread, n);
	result = sys_pread(fsp->fh->fd, data, n, offset);
	END_PROFILE(syscall_pread);

	if (result == -1 && errno == ESPIPE) {
		/* Maintain the fiction that pipes can be seeked (sought?) on. */
		result = SMB_VFS_READ(fsp, data, n);
		fsp->fh->pos = 0;
	}

#else /* HAVE_PREAD */
	off_t   curr;
	int lerrno;

	curr = SMB_VFS_LSEEK(fsp, 0, SEEK_CUR);
	if (curr == -1 && errno == ESPIPE) {
		/* Maintain the fiction that pipes can be seeked (sought?) on. */
		result = SMB_VFS_READ(fsp, data, n);
		fsp->fh->pos = 0;
		return result;
	}

	if (SMB_VFS_LSEEK(fsp, offset, SEEK_SET) == -1) {
		return -1;
	}

	errno = 0;
	result = SMB_VFS_READ(fsp, data, n);
	lerrno = errno;

	SMB_VFS_LSEEK(fsp, curr, SEEK_SET);
	errno = lerrno;

#endif /* HAVE_PREAD */

	return result;
}
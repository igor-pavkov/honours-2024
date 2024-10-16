static int vfswrap_fchmod(vfs_handle_struct *handle, files_struct *fsp, mode_t mode)
{
	int result;

	START_PROFILE(syscall_fchmod);

	/*
	 * We need to do this due to the fact that the default POSIX ACL
	 * chmod modifies the ACL *mask* for the group owner, not the
	 * group owner bits directly. JRA.
	 */

	{
		int saved_errno = errno; /* We might get ENOSYS */
		if ((result = SMB_VFS_FCHMOD_ACL(fsp, mode)) == 0) {
			END_PROFILE(syscall_fchmod);
			return result;
		}
		/* Error - return the old errno. */
		errno = saved_errno;
	}

#if defined(HAVE_FCHMOD)
	result = fchmod(fsp->fh->fd, mode);
#else
	result = -1;
	errno = ENOSYS;
#endif

	END_PROFILE(syscall_fchmod);
	return result;
}
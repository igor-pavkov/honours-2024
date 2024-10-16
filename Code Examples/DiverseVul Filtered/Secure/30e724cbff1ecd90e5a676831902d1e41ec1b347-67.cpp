static struct dirent *vfswrap_readdir(vfs_handle_struct *handle,
				          DIR *dirp,
					  SMB_STRUCT_STAT *sbuf)
{
	struct dirent *result;

	START_PROFILE(syscall_readdir);
	result = readdir(dirp);
	END_PROFILE(syscall_readdir);
	if (sbuf) {
		/* Default Posix readdir() does not give us stat info.
		 * Set to invalid to indicate we didn't return this info. */
		SET_STAT_INVALID(*sbuf);
#if defined(HAVE_DIRFD) && defined(HAVE_FSTATAT)
		if (result != NULL) {
			/* See if we can efficiently return this. */
			struct stat st;
			int flags = (lp_posix_pathnames() ?
				AT_SYMLINK_NOFOLLOW : 0);
			int ret = fstatat(dirfd(dirp),
					result->d_name,
					&st,
					flags);
			if (ret == 0) {
				init_stat_ex_from_stat(sbuf,
					&st,
					lp_fake_dir_create_times(
						SNUM(handle->conn)));
			}
		}
#endif
	}
	return result;
}
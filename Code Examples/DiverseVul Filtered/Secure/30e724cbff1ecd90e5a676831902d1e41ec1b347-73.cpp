static int vfswrap_ntimes(vfs_handle_struct *handle,
			  const struct smb_filename *smb_fname,
			  struct smb_file_time *ft)
{
	int result = -1;

	START_PROFILE(syscall_ntimes);

	if (smb_fname->stream_name) {
		errno = ENOENT;
		goto out;
	}

	if (ft != NULL) {
		if (null_timespec(ft->atime)) {
			ft->atime= smb_fname->st.st_ex_atime;
		}

		if (null_timespec(ft->mtime)) {
			ft->mtime = smb_fname->st.st_ex_mtime;
		}

		if (!null_timespec(ft->create_time)) {
			set_create_timespec_ea(handle->conn,
					       smb_fname,
					       ft->create_time);
		}

		if ((timespec_compare(&ft->atime,
				      &smb_fname->st.st_ex_atime) == 0) &&
		    (timespec_compare(&ft->mtime,
				      &smb_fname->st.st_ex_mtime) == 0)) {
			return 0;
		}
	}

#if defined(HAVE_UTIMENSAT)
	if (ft != NULL) {
		struct timespec ts[2];
		ts[0] = ft->atime;
		ts[1] = ft->mtime;
		result = utimensat(AT_FDCWD, smb_fname->base_name, ts, 0);
	} else {
		result = utimensat(AT_FDCWD, smb_fname->base_name, NULL, 0);
	}
	if (!((result == -1) && (errno == ENOSYS))) {
		goto out;
	}
#endif
#if defined(HAVE_UTIMES)
	if (ft != NULL) {
		struct timeval tv[2];
		tv[0] = convert_timespec_to_timeval(ft->atime);
		tv[1] = convert_timespec_to_timeval(ft->mtime);
		result = utimes(smb_fname->base_name, tv);
	} else {
		result = utimes(smb_fname->base_name, NULL);
	}
	if (!((result == -1) && (errno == ENOSYS))) {
		goto out;
	}
#endif
#if defined(HAVE_UTIME)
	if (ft != NULL) {
		struct utimbuf times;
		times.actime = convert_timespec_to_time_t(ft->atime);
		times.modtime = convert_timespec_to_time_t(ft->mtime);
		result = utime(smb_fname->base_name, &times);
	} else {
		result = utime(smb_fname->base_name, NULL);
	}
	if (!((result == -1) && (errno == ENOSYS))) {
		goto out;
	}
#endif
	errno = ENOSYS;
	result = -1;

 out:
	END_PROFILE(syscall_ntimes);
	return result;
}
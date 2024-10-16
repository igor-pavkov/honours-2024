static uint32_t vfswrap_fs_capabilities(struct vfs_handle_struct *handle,
		enum timestamp_set_resolution *p_ts_res)
{
	connection_struct *conn = handle->conn;
	uint32_t caps = FILE_CASE_SENSITIVE_SEARCH | FILE_CASE_PRESERVED_NAMES;
	struct smb_filename *smb_fname_cpath = NULL;
	struct vfs_statvfs_struct statbuf;
	int ret;

	ZERO_STRUCT(statbuf);
	ret = sys_statvfs(conn->connectpath, &statbuf);
	if (ret == 0) {
		caps = statbuf.FsCapabilities;
	}

	*p_ts_res = TIMESTAMP_SET_SECONDS;

	/* Work out what timestamp resolution we can
	 * use when setting a timestamp. */

	smb_fname_cpath = synthetic_smb_fname(talloc_tos(), conn->connectpath,
					      NULL, NULL);
	if (smb_fname_cpath == NULL) {
		return caps;
	}

	ret = SMB_VFS_STAT(conn, smb_fname_cpath);
	if (ret == -1) {
		TALLOC_FREE(smb_fname_cpath);
		return caps;
	}

	if (smb_fname_cpath->st.st_ex_mtime.tv_nsec ||
			smb_fname_cpath->st.st_ex_atime.tv_nsec ||
			smb_fname_cpath->st.st_ex_ctime.tv_nsec) {
		/* If any of the normal UNIX directory timestamps
		 * have a non-zero tv_nsec component assume
		 * we might be able to set sub-second timestamps.
		 * See what filetime set primitives we have.
		 */
#if defined(HAVE_UTIMENSAT)
		*p_ts_res = TIMESTAMP_SET_NT_OR_BETTER;
#elif defined(HAVE_UTIMES)
		/* utimes allows msec timestamps to be set. */
		*p_ts_res = TIMESTAMP_SET_MSEC;
#elif defined(HAVE_UTIME)
		/* utime only allows sec timestamps to be set. */
		*p_ts_res = TIMESTAMP_SET_SECONDS;
#endif

		DEBUG(10,("vfswrap_fs_capabilities: timestamp "
			"resolution of %s "
			"available on share %s, directory %s\n",
			*p_ts_res == TIMESTAMP_SET_MSEC ? "msec" : "sec",
			lp_servicename(talloc_tos(), conn->params->service),
			conn->connectpath ));
	}
	TALLOC_FREE(smb_fname_cpath);
	return caps;
}
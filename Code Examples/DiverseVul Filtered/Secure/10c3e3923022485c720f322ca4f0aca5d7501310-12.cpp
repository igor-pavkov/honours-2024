static NTSTATUS fd_open_atomic(struct connection_struct *conn,
			files_struct *fsp,
			int flags,
			mode_t mode,
			bool *file_created)
{
	NTSTATUS status = NT_STATUS_UNSUCCESSFUL;
	NTSTATUS retry_status;
	bool file_existed = VALID_STAT(fsp->fsp_name->st);
	int curr_flags;

	*file_created = false;

	if (!(flags & O_CREAT)) {
		/*
		 * We're not creating the file, just pass through.
		 */
		return fd_open(conn, fsp, flags, mode);
	}

	if (flags & O_EXCL) {
		/*
		 * Fail if already exists, just pass through.
		 */
		status = fd_open(conn, fsp, flags, mode);

		/*
		 * Here we've opened with O_CREAT|O_EXCL. If that went
		 * NT_STATUS_OK, we *know* we created this file.
		 */
		*file_created = NT_STATUS_IS_OK(status);

		return status;
	}

	/*
	 * Now it gets tricky. We have O_CREAT, but not O_EXCL.
	 * To know absolutely if we created the file or not,
	 * we can never call O_CREAT without O_EXCL. So if
	 * we think the file existed, try without O_CREAT|O_EXCL.
	 * If we think the file didn't exist, try with
	 * O_CREAT|O_EXCL.
	 *
	 * The big problem here is dangling symlinks. Opening
	 * without O_NOFOLLOW means both bad symlink
	 * and missing path return -1, ENOENT from open(). As POSIX
	 * is pathname based it's not possible to tell
	 * the difference between these two cases in a
	 * non-racy way, so change to try only two attempts before
	 * giving up.
	 *
	 * We don't have this problem for the O_NOFOLLOW
	 * case as it just returns NT_STATUS_OBJECT_PATH_NOT_FOUND
	 * mapped from the ELOOP POSIX error.
	 */

	curr_flags = flags;

	if (file_existed) {
		curr_flags &= ~(O_CREAT);
		retry_status = NT_STATUS_OBJECT_NAME_NOT_FOUND;
	} else {
		curr_flags |= O_EXCL;
		retry_status = NT_STATUS_OBJECT_NAME_COLLISION;
	}

	status = fd_open(conn, fsp, curr_flags, mode);
	if (NT_STATUS_IS_OK(status)) {
		if (!file_existed) {
			*file_created = true;
		}
		return NT_STATUS_OK;
	}
	if (!NT_STATUS_EQUAL(status, retry_status)) {
		return status;
	}

	curr_flags = flags;

	/*
	 * Keep file_existed up to date for clarity.
	 */
	if (NT_STATUS_EQUAL(status, NT_STATUS_OBJECT_NAME_NOT_FOUND)) {
		file_existed = false;
		curr_flags |= O_EXCL;
		DBG_DEBUG("file %s did not exist. Retry.\n",
			smb_fname_str_dbg(fsp->fsp_name));
	} else {
		file_existed = true;
		curr_flags &= ~(O_CREAT);
		DBG_DEBUG("file %s existed. Retry.\n",
			smb_fname_str_dbg(fsp->fsp_name));
	}

	status = fd_open(conn, fsp, curr_flags, mode);

	if (NT_STATUS_IS_OK(status) && (!file_existed)) {
		*file_created = true;
	}

	return status;
}
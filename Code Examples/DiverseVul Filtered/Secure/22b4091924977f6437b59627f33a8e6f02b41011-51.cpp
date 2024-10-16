static NTSTATUS smb_query_posix_acl(connection_struct *conn,
				struct smb_request *req,
				files_struct *fsp,
				struct smb_filename *smb_fname,
				char *pdata,
				unsigned int data_size_in,
				unsigned int *pdata_size_out)
{
	SMB_ACL_T file_acl = NULL;
	SMB_ACL_T def_acl = NULL;
	uint16_t num_file_acls = 0;
	uint16_t num_def_acls = 0;
	unsigned int size_needed = 0;
	NTSTATUS status;
	bool ok;
	bool close_fsp = false;

	/*
	 * Ensure we always operate on a file descriptor, not just
	 * the filename.
	 */
	if (fsp == NULL || !fsp->fsp_flags.is_fsa) {
		uint32_t access_mask = SEC_STD_READ_CONTROL|
					FILE_READ_ATTRIBUTES|
					FILE_WRITE_ATTRIBUTES;

		status = get_posix_fsp(conn,
					req,
					smb_fname,
					access_mask,
					&fsp);

		if (!NT_STATUS_IS_OK(status)) {
			goto out;
		}
		close_fsp = true;
	}

	SMB_ASSERT(fsp != NULL);

	status = refuse_symlink_fsp(fsp);
	if (!NT_STATUS_IS_OK(status)) {
		goto out;
	}

	file_acl = SMB_VFS_SYS_ACL_GET_FD(fsp, SMB_ACL_TYPE_ACCESS,
					talloc_tos());

	if (file_acl == NULL && no_acl_syscall_error(errno)) {
		DBG_INFO("ACLs not implemented on "
			"filesystem containing %s\n",
			fsp_str_dbg(fsp));
		status = NT_STATUS_NOT_IMPLEMENTED;
		goto out;
	}

	if (S_ISDIR(fsp->fsp_name->st.st_ex_mode)) {
		/*
		 * We can only have default POSIX ACLs on
		 * directories.
		 */
		if (!fsp->fsp_flags.is_directory) {
			DBG_INFO("Non-directory open %s\n",
				fsp_str_dbg(fsp));
			status = NT_STATUS_INVALID_HANDLE;
			goto out;
		}
		def_acl = SMB_VFS_SYS_ACL_GET_FD(fsp,
					SMB_ACL_TYPE_DEFAULT,
					talloc_tos());
		def_acl = free_empty_sys_acl(conn, def_acl);
	}

	num_file_acls = count_acl_entries(conn, file_acl);
	num_def_acls = count_acl_entries(conn, def_acl);

	/* Wrap checks. */
	if (num_file_acls + num_def_acls < num_file_acls) {
		status = NT_STATUS_INVALID_PARAMETER;
		goto out;
	}

	size_needed = num_file_acls + num_def_acls;

	/*
	 * (size_needed * SMB_POSIX_ACL_ENTRY_SIZE) must be less
	 * than UINT_MAX, so check by division.
	 */
	if (size_needed > (UINT_MAX/SMB_POSIX_ACL_ENTRY_SIZE)) {
		status = NT_STATUS_INVALID_PARAMETER;
		goto out;
	}

	size_needed = size_needed*SMB_POSIX_ACL_ENTRY_SIZE;
	if (size_needed + SMB_POSIX_ACL_HEADER_SIZE < size_needed) {
		status = NT_STATUS_INVALID_PARAMETER;
		goto out;
	}
	size_needed += SMB_POSIX_ACL_HEADER_SIZE;

	if ( data_size_in < size_needed) {
		DBG_INFO("data_size too small (%u) need %u\n",
			data_size_in,
			size_needed);
		status = NT_STATUS_BUFFER_TOO_SMALL;
		goto out;
	}

	SSVAL(pdata,0,SMB_POSIX_ACL_VERSION);
	SSVAL(pdata,2,num_file_acls);
	SSVAL(pdata,4,num_def_acls);
	pdata += SMB_POSIX_ACL_HEADER_SIZE;

	ok = marshall_posix_acl(conn,
			pdata,
			&fsp->fsp_name->st,
			file_acl);
	if (!ok) {
		status = NT_STATUS_INTERNAL_ERROR;
		goto out;
	}
	pdata += (num_file_acls*SMB_POSIX_ACL_ENTRY_SIZE);

	ok = marshall_posix_acl(conn,
			pdata,
			&fsp->fsp_name->st,
			def_acl);
	if (!ok) {
		status = NT_STATUS_INTERNAL_ERROR;
		goto out;
	}

	*pdata_size_out = size_needed;
	status = NT_STATUS_OK;

  out:

	if (close_fsp) {
		/*
		 * Ensure the stat struct in smb_fname is up to
		 * date. Structure copy.
		 */
		smb_fname->st = fsp->fsp_name->st;
		(void)close_file(req, fsp, NORMAL_CLOSE);
		fsp = NULL;
	}

	TALLOC_FREE(file_acl);
	TALLOC_FREE(def_acl);
	return status;
}
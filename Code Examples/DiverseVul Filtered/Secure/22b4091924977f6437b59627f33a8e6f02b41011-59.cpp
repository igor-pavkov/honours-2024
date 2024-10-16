static NTSTATUS smb_set_posix_acl(connection_struct *conn,
				struct smb_request *req,
				const char *pdata,
				int total_data_in,
				files_struct *fsp,
				struct smb_filename *smb_fname)
{
	uint16_t posix_acl_version;
	uint16_t num_file_acls;
	uint16_t num_def_acls;
	bool valid_file_acls = true;
	bool valid_def_acls = true;
	NTSTATUS status;
	unsigned int size_needed;
	unsigned int total_data;
	bool close_fsp = false;

	if (total_data_in < 0) {
		status = NT_STATUS_INVALID_PARAMETER;
		goto out;
	}

	total_data = total_data_in;

	if (total_data < SMB_POSIX_ACL_HEADER_SIZE) {
		status = NT_STATUS_INVALID_PARAMETER;
		goto out;
	}
	posix_acl_version = SVAL(pdata,0);
	num_file_acls = SVAL(pdata,2);
	num_def_acls = SVAL(pdata,4);

	if (num_file_acls == SMB_POSIX_IGNORE_ACE_ENTRIES) {
		valid_file_acls = false;
		num_file_acls = 0;
	}

	if (num_def_acls == SMB_POSIX_IGNORE_ACE_ENTRIES) {
		valid_def_acls = false;
		num_def_acls = 0;
	}

	if (posix_acl_version != SMB_POSIX_ACL_VERSION) {
		status = NT_STATUS_INVALID_PARAMETER;
		goto out;
	}

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

	if (total_data < size_needed) {
		status = NT_STATUS_INVALID_PARAMETER;
		goto out;
	}

	/*
	 * Ensure we always operate on a file descriptor, not just
	 * the filename.
	 */
	if (fsp == NULL || !fsp->fsp_flags.is_fsa) {
		uint32_t access_mask = SEC_STD_WRITE_OWNER|
					SEC_STD_WRITE_DAC|
					SEC_STD_READ_CONTROL|
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

	/* Here we know fsp != NULL */
	SMB_ASSERT(fsp != NULL);

	status = refuse_symlink_fsp(fsp);
	if (!NT_STATUS_IS_OK(status)) {
		goto out;
	}

	/* If we have a default acl, this *must* be a directory. */
	if (valid_def_acls && !fsp->fsp_flags.is_directory) {
		DBG_INFO("Can't set default acls on "
			 "non-directory %s\n",
			 fsp_str_dbg(fsp));
		return NT_STATUS_INVALID_HANDLE;
	}

	DBG_DEBUG("file %s num_file_acls = %"PRIu16", "
		  "num_def_acls = %"PRIu16"\n",
		  fsp_str_dbg(fsp),
		  num_file_acls,
		  num_def_acls);

	/* Move pdata to the start of the file ACL entries. */
	pdata += SMB_POSIX_ACL_HEADER_SIZE;

	if (valid_file_acls) {
		status = set_unix_posix_acl(conn,
					fsp,
					num_file_acls,
					pdata);
		if (!NT_STATUS_IS_OK(status)) {
			goto out;
		}
	}

	/* Move pdata to the start of the default ACL entries. */
	pdata += (num_file_acls*SMB_POSIX_ACL_ENTRY_SIZE);

	if (valid_def_acls) {
		status = set_unix_posix_default_acl(conn,
					fsp,
					num_def_acls,
					pdata);
		if (!NT_STATUS_IS_OK(status)) {
			goto out;
		}
	}

	status = NT_STATUS_OK;

  out:

	if (close_fsp) {
		(void)close_file(req, fsp, NORMAL_CLOSE);
		fsp = NULL;
	}
	return status;
}
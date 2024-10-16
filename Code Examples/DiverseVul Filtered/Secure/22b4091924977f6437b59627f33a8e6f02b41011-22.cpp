static NTSTATUS smb_unix_read_symlink(connection_struct *conn,
				struct smb_request *req,
				struct smb_filename *smb_fname,
				char *pdata,
				unsigned int data_size_in,
				unsigned int *pdata_size_out)
{
	NTSTATUS status;
	size_t len = 0;
	int link_len = 0;
	struct smb_filename *parent_fname = NULL;
	struct smb_filename *base_name = NULL;

	char *buffer = talloc_array(talloc_tos(), char, PATH_MAX+1);

	if (!buffer) {
		return NT_STATUS_NO_MEMORY;
	}

	DBG_DEBUG("SMB_QUERY_FILE_UNIX_LINK for file %s\n",
		smb_fname_str_dbg(smb_fname));

	if(!S_ISLNK(smb_fname->st.st_ex_mode)) {
		TALLOC_FREE(buffer);
		return NT_STATUS_DOS(ERRSRV, ERRbadlink);
	}

	status = parent_pathref(talloc_tos(),
				conn->cwd_fsp,
				smb_fname,
				&parent_fname,
				&base_name);
	if (!NT_STATUS_IS_OK(status)) {
		TALLOC_FREE(buffer);
		return status;
	}

	link_len = SMB_VFS_READLINKAT(conn,
				parent_fname->fsp,
				base_name,
				buffer,
				PATH_MAX);

	TALLOC_FREE(parent_fname);

	if (link_len == -1) {
		TALLOC_FREE(buffer);
		return map_nt_error_from_unix(errno);
	}

	buffer[link_len] = 0;
	status = srvstr_push(pdata,
			req->flags2,
			pdata,
			buffer,
			data_size_in,
			STR_TERMINATE,
			&len);
	TALLOC_FREE(buffer);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}
	*pdata_size_out = len;

	return NT_STATUS_OK;
}
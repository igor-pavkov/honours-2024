static NTSTATUS smb_set_file_unix_hlink(connection_struct *conn,
					struct smb_request *req,
					const char *pdata, int total_data,
					struct smb_filename *smb_fname_new)
{
	char *oldname = NULL;
	struct smb_filename *smb_fname_old = NULL;
	uint32_t ucf_flags = ucf_flags_from_smb_request(req);
	TALLOC_CTX *ctx = talloc_tos();
	NTSTATUS status = NT_STATUS_OK;

	/* Set a hard link. */
	if (total_data == 0) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (req->posix_pathnames) {
		srvstr_get_path_posix(ctx,
			pdata,
			req->flags2,
			&oldname,
			pdata,
			total_data,
			STR_TERMINATE,
			&status);
	} else {
		srvstr_get_path(ctx,
			pdata,
			req->flags2,
			&oldname,
			pdata,
			total_data,
			STR_TERMINATE,
			&status);
	}
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	DEBUG(10,("smb_set_file_unix_hlink: SMB_SET_FILE_UNIX_LINK doing hard link %s -> %s\n",
		smb_fname_str_dbg(smb_fname_new), oldname));

	status = filename_convert(ctx,
				conn,
				oldname,
				ucf_flags,
				0,
				&smb_fname_old);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	return hardlink_internals(ctx, conn, req, false,
			smb_fname_old, smb_fname_new);
}
static NTSTATUS smb_file_link_information(connection_struct *conn,
					    struct smb_request *req,
					    const char *pdata,
					    int total_data,
					    files_struct *fsp,
					    struct smb_filename *smb_fname_src)
{
	bool overwrite;
	uint32_t len;
	char *newname = NULL;
	struct smb_filename *smb_fname_dst = NULL;
	NTSTATUS status = NT_STATUS_OK;
	uint32_t ucf_flags = ucf_flags_from_smb_request(req);
	TALLOC_CTX *ctx = talloc_tos();

	if (!fsp) {
		return NT_STATUS_INVALID_HANDLE;
	}

	if (total_data < 20) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	overwrite = (CVAL(pdata,0) ? true : false);
	len = IVAL(pdata,16);

	if (len > (total_data - 20) || (len == 0)) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (smb_fname_src->flags & SMB_FILENAME_POSIX_PATH) {
		srvstr_get_path_posix(ctx,
				pdata,
				req->flags2,
				&newname,
				&pdata[20],
				len,
				STR_TERMINATE,
				&status);
		ucf_flags |= UCF_POSIX_PATHNAMES;
	} else {
		srvstr_get_path(ctx,
				pdata,
				req->flags2,
				&newname,
				&pdata[20],
				len,
				STR_TERMINATE,
				&status);
	}
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	DEBUG(10,("smb_file_link_information: got name |%s|\n",
				newname));

	status = filename_convert(ctx,
				conn,
				newname,
				ucf_flags,
				0,
				&smb_fname_dst);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (fsp->base_fsp) {
		/* No stream names. */
		return NT_STATUS_NOT_SUPPORTED;
	}

	DEBUG(10,("smb_file_link_information: "
		  "SMB_FILE_LINK_INFORMATION (%s) %s -> %s\n",
		  fsp_fnum_dbg(fsp), fsp_str_dbg(fsp),
		  smb_fname_str_dbg(smb_fname_dst)));
	status = hardlink_internals(ctx,
				conn,
				req,
				overwrite,
				fsp->fsp_name,
				smb_fname_dst);

	TALLOC_FREE(smb_fname_dst);
	return status;
}
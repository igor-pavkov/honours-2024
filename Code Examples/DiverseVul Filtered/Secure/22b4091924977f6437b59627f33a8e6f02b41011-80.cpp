static NTSTATUS smb2_file_rename_information(connection_struct *conn,
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
	const char *dst_original_lcomp = NULL;
	uint32_t ucf_flags = ucf_flags_from_smb_request(req);
	NTSTATUS status = NT_STATUS_OK;
	TALLOC_CTX *ctx = talloc_tos();

	if (!fsp) {
		return NT_STATUS_INVALID_HANDLE;
	}

	if (total_data < 20) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	overwrite = (CVAL(pdata,0) ? True : False);
	len = IVAL(pdata,16);

	if (len > (total_data - 20) || (len == 0)) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (req->posix_pathnames) {
		srvstr_get_path_posix(ctx,
				pdata,
				req->flags2,
				&newname,
				&pdata[20],
				len,
				STR_TERMINATE,
				&status);
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

	DEBUG(10,("smb2_file_rename_information: got name |%s|\n",
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
		/* newname must be a stream name. */
		if (newname[0] != ':') {
			return NT_STATUS_NOT_SUPPORTED;
		}

		/* Create an smb_fname to call rename_internals_fsp() with. */
		smb_fname_dst = synthetic_smb_fname(talloc_tos(),
					fsp->base_fsp->fsp_name->base_name,
					newname,
					NULL,
					fsp->base_fsp->fsp_name->twrp,
					fsp->base_fsp->fsp_name->flags);
		if (smb_fname_dst == NULL) {
			status = NT_STATUS_NO_MEMORY;
			goto out;
		}
	}

	/*
	 * Set the original last component, since
	 * rename_internals_fsp() requires it.
	 */
	dst_original_lcomp = get_original_lcomp(smb_fname_dst,
					conn,
					newname,
					ucf_flags);
	if (dst_original_lcomp == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto out;
	}

	DEBUG(10,("smb2_file_rename_information: "
		  "SMB_FILE_RENAME_INFORMATION (%s) %s -> %s\n",
		  fsp_fnum_dbg(fsp), fsp_str_dbg(fsp),
		  smb_fname_str_dbg(smb_fname_dst)));
	status = rename_internals_fsp(conn,
				fsp,
				smb_fname_dst,
				dst_original_lcomp,
				(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM),
				overwrite);

 out:
	TALLOC_FREE(smb_fname_dst);
	return status;
}
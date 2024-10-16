static NTSTATUS smb_file_rename_information(connection_struct *conn,
					    struct smb_request *req,
					    const char *pdata,
					    int total_data,
					    files_struct *fsp,
					    struct smb_filename *smb_fname_src)
{
	bool overwrite;
	uint32_t root_fid;
	uint32_t len;
	char *newname = NULL;
	struct smb_filename *smb_fname_dst = NULL;
	const char *dst_original_lcomp = NULL;
	NTSTATUS status = NT_STATUS_OK;
	char *p;
	TALLOC_CTX *ctx = talloc_tos();

	if (total_data < 13) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	overwrite = (CVAL(pdata,0) ? True : False);
	root_fid = IVAL(pdata,4);
	len = IVAL(pdata,8);

	if (len > (total_data - 12) || (len == 0) || (root_fid != 0)) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (req->posix_pathnames) {
		srvstr_get_path_posix(ctx,
				pdata,
				req->flags2,
				&newname,
				&pdata[12],
				len,
				0,
				&status);
	} else {
		srvstr_get_path(ctx,
				pdata,
				req->flags2,
				&newname,
				&pdata[12],
				len,
				0,
				&status);
	}
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	DEBUG(10,("smb_file_rename_information: got name |%s|\n",
				newname));

	/* Check the new name has no '/' characters. */
	if (strchr_m(newname, '/')) {
		return NT_STATUS_NOT_SUPPORTED;
	}

	if (fsp && fsp->base_fsp) {
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

		/*
		 * Get the original last component, since
		 * rename_internals_fsp() requires it.
		 */
		dst_original_lcomp = get_original_lcomp(smb_fname_dst,
					conn,
					newname,
					0);
		if (dst_original_lcomp == NULL) {
			status = NT_STATUS_NO_MEMORY;
			goto out;
		}

	} else {
		/*
		 * Build up an smb_fname_dst based on the filename passed in.
		 * We basically just strip off the last component, and put on
		 * the newname instead.
		 */
		char *base_name = NULL;
		uint32_t ucf_flags = UCF_ALWAYS_ALLOW_WCARD_LCOMP|
				ucf_flags_from_smb_request(req);

		/* newname must *not* be a stream name. */
		if (newname[0] == ':') {
			return NT_STATUS_NOT_SUPPORTED;
		}

		/*
		 * Strip off the last component (filename) of the path passed
		 * in.
		 */
		base_name = talloc_strdup(ctx, smb_fname_src->base_name);
		if (!base_name) {
			return NT_STATUS_NO_MEMORY;
		}
		p = strrchr_m(base_name, '/');
		if (p) {
			p[1] = '\0';
		} else {
			base_name = talloc_strdup(ctx, "");
			if (!base_name) {
				return NT_STATUS_NO_MEMORY;
			}
		}
		/* Append the new name. */
		base_name = talloc_asprintf_append(base_name,
				"%s",
				newname);
		if (!base_name) {
			return NT_STATUS_NO_MEMORY;
		}

		status = filename_convert(ctx,
					  conn,
					  base_name,
					  ucf_flags,
					  0,
					  &smb_fname_dst);

		/* If an error we expect this to be
		 * NT_STATUS_OBJECT_PATH_NOT_FOUND */

		if (!NT_STATUS_IS_OK(status)) {
			if(!NT_STATUS_EQUAL(NT_STATUS_OBJECT_PATH_NOT_FOUND,
					    status)) {
				goto out;
			}
			/* Create an smb_fname to call rename_internals_fsp() */
			smb_fname_dst = synthetic_smb_fname(ctx,
						base_name,
						NULL,
						NULL,
						smb_fname_src->twrp,
						smb_fname_src->flags);
			if (smb_fname_dst == NULL) {
				status = NT_STATUS_NO_MEMORY;
				goto out;
			}
		}
		dst_original_lcomp = get_original_lcomp(smb_fname_dst,
					conn,
					newname,
					ucf_flags);
		if (dst_original_lcomp == NULL) {
			status = NT_STATUS_NO_MEMORY;
			goto out;
		}
	}

	if (fsp != NULL && fsp->fsp_flags.is_fsa) {
		DEBUG(10,("smb_file_rename_information: "
			  "SMB_FILE_RENAME_INFORMATION (%s) %s -> %s\n",
			  fsp_fnum_dbg(fsp), fsp_str_dbg(fsp),
			  smb_fname_str_dbg(smb_fname_dst)));
		status = rename_internals_fsp(conn,
					fsp,
					smb_fname_dst,
					dst_original_lcomp,
					0,
					overwrite);
	} else {
		DEBUG(10,("smb_file_rename_information: "
			  "SMB_FILE_RENAME_INFORMATION %s -> %s\n",
			  smb_fname_str_dbg(smb_fname_src),
			  smb_fname_str_dbg(smb_fname_dst)));
		status = rename_internals(ctx,
					conn,
					req,
					smb_fname_src,
					NULL,
					smb_fname_dst,
					dst_original_lcomp,
					0,
					overwrite,
					FILE_WRITE_ATTRIBUTES);
	}
 out:
	TALLOC_FREE(smb_fname_dst);
	return status;
}
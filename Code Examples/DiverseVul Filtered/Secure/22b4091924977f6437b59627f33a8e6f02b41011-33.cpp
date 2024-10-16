static NTSTATUS smb_set_file_unix_link(connection_struct *conn,
				       struct smb_request *req,
				       const char *pdata,
				       int total_data,
				       struct smb_filename *new_smb_fname)
{
	char *link_target = NULL;
	struct smb_filename target_fname;
	TALLOC_CTX *ctx = talloc_tos();
	NTSTATUS status;
	int ret;
	struct smb_filename *parent_fname = NULL;
	struct smb_filename *base_name = NULL;

	/* Set a symbolic link. */
	/* Don't allow this if follow links is false. */

	if (total_data == 0) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (!lp_follow_symlinks(SNUM(conn))) {
		return NT_STATUS_ACCESS_DENIED;
	}

	srvstr_pull_talloc(ctx, pdata, req->flags2, &link_target, pdata,
		    total_data, STR_TERMINATE);

	if (!link_target) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	target_fname = (struct smb_filename) {
		.base_name = link_target,
	};

	/* Removes @GMT tokens if any */
	status = canonicalize_snapshot_path(&target_fname, UCF_GMT_PATHNAME, 0);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	DEBUG(10,("smb_set_file_unix_link: SMB_SET_FILE_UNIX_LINK doing symlink %s -> %s\n",
			new_smb_fname->base_name, link_target ));

	status = parent_pathref(talloc_tos(),
				conn->cwd_fsp,
				new_smb_fname,
				&parent_fname,
				&base_name);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	ret = SMB_VFS_SYMLINKAT(conn,
			&target_fname,
			parent_fname->fsp,
			base_name);
	if (ret != 0) {
		TALLOC_FREE(parent_fname);
		return map_nt_error_from_unix(errno);
	}

	TALLOC_FREE(parent_fname);
	return NT_STATUS_OK;
}
NTSTATUS hardlink_internals(TALLOC_CTX *ctx,
		connection_struct *conn,
		struct smb_request *req,
		bool overwrite_if_exists,
		const struct smb_filename *smb_fname_old,
		struct smb_filename *smb_fname_new)
{
	NTSTATUS status = NT_STATUS_OK;
	int ret;
	bool ok;
	struct smb_filename *parent_fname_old = NULL;
	struct smb_filename *base_name_old = NULL;
	struct smb_filename *parent_fname_new = NULL;
	struct smb_filename *base_name_new = NULL;

	/* source must already exist. */
	if (!VALID_STAT(smb_fname_old->st)) {
		status = NT_STATUS_OBJECT_NAME_NOT_FOUND;
		goto out;
	}

	/* No links from a directory. */
	if (S_ISDIR(smb_fname_old->st.st_ex_mode)) {
		status = NT_STATUS_FILE_IS_A_DIRECTORY;
		goto out;
	}

	/* Setting a hardlink to/from a stream isn't currently supported. */
	ok = is_ntfs_stream_smb_fname(smb_fname_old);
	if (ok) {
		DBG_DEBUG("Old name has streams\n");
		status = NT_STATUS_INVALID_PARAMETER;
		goto out;
	}
	ok = is_ntfs_stream_smb_fname(smb_fname_new);
	if (ok) {
		DBG_DEBUG("New name has streams\n");
		status = NT_STATUS_INVALID_PARAMETER;
		goto out;
	}

	status = parent_pathref(talloc_tos(),
				conn->cwd_fsp,
				smb_fname_old,
				&parent_fname_old,
				&base_name_old);
	if (!NT_STATUS_IS_OK(status)) {
		goto out;
	}

	status = parent_pathref(talloc_tos(),
				conn->cwd_fsp,
				smb_fname_new,
				&parent_fname_new,
				&base_name_new);
	if (!NT_STATUS_IS_OK(status)) {
		goto out;
	}

	if (VALID_STAT(smb_fname_new->st)) {
		if (overwrite_if_exists) {
			if (S_ISDIR(smb_fname_new->st.st_ex_mode)) {
				status = NT_STATUS_FILE_IS_A_DIRECTORY;
				goto out;
			}
			status = unlink_internals(conn,
						req,
						FILE_ATTRIBUTE_NORMAL,
						smb_fname_new,
						false);
			if (!NT_STATUS_IS_OK(status)) {
				goto out;
			}
		} else {
			/* Disallow if newname already exists. */
			status = NT_STATUS_OBJECT_NAME_COLLISION;
			goto out;
		}
	}

	DEBUG(10,("hardlink_internals: doing hard link %s -> %s\n",
		  smb_fname_old->base_name, smb_fname_new->base_name));

	ret = SMB_VFS_LINKAT(conn,
			parent_fname_old->fsp,
			base_name_old,
			parent_fname_new->fsp,
			base_name_new,
			0);

	if (ret != 0) {
		status = map_nt_error_from_unix(errno);
		DEBUG(3,("hardlink_internals: Error %s hard link %s -> %s\n",
			 nt_errstr(status), smb_fname_old->base_name,
			 smb_fname_new->base_name));
	}

  out:

	TALLOC_FREE(parent_fname_old);
	TALLOC_FREE(parent_fname_new);
	return status;
}
static NTSTATUS smb_posix_unlink(connection_struct *conn,
				 struct smb_request *req,
				const char *pdata,
				int total_data,
				struct smb_filename *smb_fname)
{
	NTSTATUS status = NT_STATUS_OK;
	files_struct *fsp = NULL;
	uint16_t flags = 0;
	char del = 1;
	int info = 0;
	int create_options = 0;
	struct share_mode_lock *lck = NULL;
	bool other_nonposix_opens;
	struct smb2_create_blobs *posx = NULL;

	if (total_data < 2) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	flags = SVAL(pdata,0);

	if (!VALID_STAT(smb_fname->st)) {
		return NT_STATUS_OBJECT_NAME_NOT_FOUND;
	}

	if ((flags == SMB_POSIX_UNLINK_DIRECTORY_TARGET) &&
			!VALID_STAT_OF_DIR(smb_fname->st)) {
		return NT_STATUS_NOT_A_DIRECTORY;
	}

	DEBUG(10,("smb_posix_unlink: %s %s\n",
		(flags == SMB_POSIX_UNLINK_DIRECTORY_TARGET) ? "directory" : "file",
		smb_fname_str_dbg(smb_fname)));

	if (VALID_STAT_OF_DIR(smb_fname->st)) {
		create_options |= FILE_DIRECTORY_FILE;
	}

	status = make_smb2_posix_create_ctx(talloc_tos(), &posx, 0777);
	if (!NT_STATUS_IS_OK(status)) {
		DBG_WARNING("make_smb2_posix_create_ctx failed: %s\n",
			    nt_errstr(status));
		return status;
	}

        status = SMB_VFS_CREATE_FILE(
		conn,					/* conn */
		req,					/* req */
		smb_fname,				/* fname */
		DELETE_ACCESS,				/* access_mask */
		(FILE_SHARE_READ | FILE_SHARE_WRITE |	/* share_access */
		    FILE_SHARE_DELETE),
		FILE_OPEN,				/* create_disposition*/
		create_options,				/* create_options */
		0,					/* file_attributes */
		0,					/* oplock_request */
		NULL,					/* lease */
		0,					/* allocation_size */
		0,					/* private_flags */
		NULL,					/* sd */
		NULL,					/* ea_list */
		&fsp,					/* result */
		&info,					/* pinfo */
		posx,					/* in_context_blobs */
		NULL);					/* out_context_blobs */

	TALLOC_FREE(posx);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/*
	 * Don't lie to client. If we can't really delete due to
	 * non-POSIX opens return SHARING_VIOLATION.
	 */

	lck = get_existing_share_mode_lock(talloc_tos(), fsp->file_id);
	if (lck == NULL) {
		DEBUG(0, ("smb_posix_unlink: Could not get share mode "
			  "lock for file %s\n", fsp_str_dbg(fsp)));
		close_file(req, fsp, NORMAL_CLOSE);
		return NT_STATUS_INVALID_PARAMETER;
	}

	other_nonposix_opens = has_other_nonposix_opens(lck, fsp);
	if (other_nonposix_opens) {
		/* Fail with sharing violation. */
		TALLOC_FREE(lck);
		close_file(req, fsp, NORMAL_CLOSE);
		return NT_STATUS_SHARING_VIOLATION;
	}

	/*
	 * Set the delete on close.
	 */
	status = smb_set_file_disposition_info(conn,
						&del,
						1,
						fsp,
						smb_fname);

	TALLOC_FREE(lck);

	if (!NT_STATUS_IS_OK(status)) {
		close_file(req, fsp, NORMAL_CLOSE);
		return status;
	}
	return close_file(req, fsp, NORMAL_CLOSE);
}
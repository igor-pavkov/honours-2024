static NTSTATUS get_posix_fsp(connection_struct *conn,
			struct smb_request *req,
			struct smb_filename *smb_fname,
			uint32_t access_mask,
			files_struct **ret_fsp)
{
	NTSTATUS status;
	uint32_t create_disposition = FILE_OPEN;
	uint32_t share_access = FILE_SHARE_READ|
				FILE_SHARE_WRITE|
				FILE_SHARE_DELETE;
	struct smb2_create_blobs *posx = NULL;

	/*
	 * Only FILE_FLAG_POSIX_SEMANTICS matters on existing files,
	 * but set reasonable defaults.
	 */
	uint32_t file_attributes = 0664;
	uint32_t oplock = NO_OPLOCK;
	uint32_t create_options = FILE_NON_DIRECTORY_FILE;

	/* File or directory must exist. */
	if (!VALID_STAT(smb_fname->st)) {
		return NT_STATUS_OBJECT_NAME_NOT_FOUND;
	}
	/* Cannot be a symlink. */
	if (S_ISLNK(smb_fname->st.st_ex_mode)) {
		return NT_STATUS_ACCESS_DENIED;
	}
	/* Set options correctly for directory open. */
	if (S_ISDIR(smb_fname->st.st_ex_mode)) {
		/*
		 * Only FILE_FLAG_POSIX_SEMANTICS matters on existing
		 * directories, but set reasonable defaults.
		 */
		file_attributes = 0775;
		create_options = FILE_DIRECTORY_FILE;
	}

	status = make_smb2_posix_create_ctx(
		talloc_tos(), &posx, file_attributes);
	if (!NT_STATUS_IS_OK(status)) {
		DBG_WARNING("make_smb2_posix_create_ctx failed: %s\n",
			    nt_errstr(status));
		goto done;
	}

	status = SMB_VFS_CREATE_FILE(
		conn,           /* conn */
		req,            /* req */
		smb_fname,      /* fname */
		access_mask,    /* access_mask */
		share_access,   /* share_access */
		create_disposition,/* create_disposition*/
		create_options, /* create_options */
		file_attributes,/* file_attributes */
		oplock,         /* oplock_request */
		NULL,           /* lease */
		0,              /* allocation_size */
		0,              /* private_flags */
		NULL,           /* sd */
		NULL,           /* ea_list */
		ret_fsp,	/* result */
		NULL,           /* pinfo */
		posx,           /* in_context */
		NULL);          /* out_context */

done:
	TALLOC_FREE(posx);
	return status;
}
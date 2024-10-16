static NTSTATUS smb_posix_mkdir(connection_struct *conn,
				struct smb_request *req,
				char **ppdata,
				int total_data,
				struct smb_filename *smb_fname,
				int *pdata_return_size)
{
	NTSTATUS status = NT_STATUS_OK;
	uint32_t raw_unixmode = 0;
	mode_t unixmode = (mode_t)0;
	files_struct *fsp = NULL;
	uint16_t info_level_return = 0;
	int info;
	char *pdata = *ppdata;
	struct smb2_create_blobs *posx = NULL;

	if (total_data < 18) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	raw_unixmode = IVAL(pdata,8);
	/* Next 4 bytes are not yet defined. */

	status = unix_perms_from_wire(conn, &smb_fname->st, raw_unixmode,
				      PERM_NEW_DIR, &unixmode);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	status = make_smb2_posix_create_ctx(talloc_tos(), &posx, unixmode);
	if (!NT_STATUS_IS_OK(status)) {
		DBG_WARNING("make_smb2_posix_create_ctx failed: %s\n",
			    nt_errstr(status));
		return status;
	}

	DEBUG(10,("smb_posix_mkdir: file %s, mode 0%o\n",
		  smb_fname_str_dbg(smb_fname), (unsigned int)unixmode));

        status = SMB_VFS_CREATE_FILE(
		conn,					/* conn */
		req,					/* req */
		smb_fname,				/* fname */
		FILE_READ_ATTRIBUTES,			/* access_mask */
		FILE_SHARE_NONE,			/* share_access */
		FILE_CREATE,				/* create_disposition*/
		FILE_DIRECTORY_FILE,			/* create_options */
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

        if (NT_STATUS_IS_OK(status)) {
                close_file(req, fsp, NORMAL_CLOSE);
        }

	info_level_return = SVAL(pdata,16);
 
	if (info_level_return == SMB_QUERY_FILE_UNIX_BASIC) {
		*pdata_return_size = 12 + SMB_FILE_UNIX_BASIC_SIZE;
	} else if (info_level_return ==  SMB_QUERY_FILE_UNIX_INFO2) {
		*pdata_return_size = 12 + SMB_FILE_UNIX_INFO2_SIZE;
	} else {
		*pdata_return_size = 12;
	}

	/* Realloc the data size */
	*ppdata = (char *)SMB_REALLOC(*ppdata,*pdata_return_size);
	if (*ppdata == NULL) {
		*pdata_return_size = 0;
		return NT_STATUS_NO_MEMORY;
	}
	pdata = *ppdata;

	SSVAL(pdata,0,NO_OPLOCK_RETURN);
	SSVAL(pdata,2,0); /* No fnum. */
	SIVAL(pdata,4,info); /* Was directory created. */

	switch (info_level_return) {
		case SMB_QUERY_FILE_UNIX_BASIC:
			SSVAL(pdata,8,SMB_QUERY_FILE_UNIX_BASIC);
			SSVAL(pdata,10,0); /* Padding. */
			store_file_unix_basic(conn, pdata + 12, fsp,
					      &smb_fname->st);
			break;
		case SMB_QUERY_FILE_UNIX_INFO2:
			SSVAL(pdata,8,SMB_QUERY_FILE_UNIX_INFO2);
			SSVAL(pdata,10,0); /* Padding. */
			store_file_unix_basic_info2(conn, pdata + 12, fsp,
						    &smb_fname->st);
			break;
		default:
			SSVAL(pdata,8,SMB_NO_INFO_LEVEL_RETURNED);
			SSVAL(pdata,10,0); /* Padding. */
			break;
	}

	return status;
}
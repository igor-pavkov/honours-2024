static NTSTATUS smbd_do_posix_setfilepathinfo(struct connection_struct *conn,
					      struct smb_request *req,
					      TALLOC_CTX *mem_ctx,
					      uint16_t info_level,
					      struct smb_filename *smb_fname,
					      files_struct *fsp,
					      char **ppdata,
					      int total_data,
					      int *ret_data_size)
{
	char *pdata = *ppdata;
	NTSTATUS status = NT_STATUS_OK;
	int data_return_size = 0;

	*ret_data_size = 0;

	if (!CAN_WRITE(conn)) {
		/* Allow POSIX opens. The open path will deny
		 * any non-readonly opens. */
		if (info_level != SMB_POSIX_PATH_OPEN) {
			return NT_STATUS_DOS(ERRSRV, ERRaccess);
		}
	}

	DBG_DEBUG("file=%s (%s) info_level=%d totdata=%d\n",
		  smb_fname_str_dbg(smb_fname),
		  fsp_fnum_dbg(fsp),
		  info_level,
		  total_data);

	switch (info_level) {
		case SMB_SET_FILE_UNIX_BASIC:
		{
			status = smb_set_file_unix_basic(conn, req,
							pdata,
							total_data,
							fsp,
							smb_fname);
			break;
		}

		case SMB_SET_FILE_UNIX_INFO2:
		{
			status = smb_set_file_unix_info2(conn, req,
							pdata,
							total_data,
							fsp,
							smb_fname);
			break;
		}

		case SMB_SET_FILE_UNIX_LINK:
		{
			if (smb_fname == NULL) {
				/* We must have a pathname for this. */
				return NT_STATUS_INVALID_LEVEL;
			}
			status = smb_set_file_unix_link(conn, req, pdata,
							total_data, smb_fname);
			break;
		}

		case SMB_SET_FILE_UNIX_HLINK:
		{
			if (smb_fname == NULL) {
				/* We must have a pathname for this. */
				return NT_STATUS_INVALID_LEVEL;
			}
			status = smb_set_file_unix_hlink(conn, req,
							 pdata,	total_data,
							 smb_fname);
			break;
		}

#if defined(HAVE_POSIX_ACLS)
		case SMB_SET_POSIX_ACL:
		{
			status = smb_set_posix_acl(conn,
						req,
						pdata,
						total_data,
						fsp,
						smb_fname);
			break;
		}
#endif

		case SMB_SET_POSIX_LOCK:
		{
			if (fsp == NULL) {
				return NT_STATUS_INVALID_LEVEL;
			}
			status = smb_set_posix_lock(conn, req,
						    pdata, total_data, fsp);
			break;
		}

		case SMB_POSIX_PATH_OPEN:
		{
			if (smb_fname == NULL) {
				/* We must have a pathname for this. */
				return NT_STATUS_INVALID_LEVEL;
			}

			status = smb_posix_open(conn, req,
						ppdata,
						total_data,
						smb_fname,
						&data_return_size);
			break;
		}

		case SMB_POSIX_PATH_UNLINK:
		{
			if (smb_fname == NULL) {
				/* We must have a pathname for this. */
				return NT_STATUS_INVALID_LEVEL;
			}

			status = smb_posix_unlink(conn, req,
						pdata,
						total_data,
						smb_fname);
			break;
		}

		default:
			return NT_STATUS_INVALID_LEVEL;
	}

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	*ret_data_size = data_return_size;
	return NT_STATUS_OK;
}
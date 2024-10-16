static NTSTATUS smb_set_file_disposition_info(connection_struct *conn,
				const char *pdata,
				int total_data,
				files_struct *fsp,
				struct smb_filename *smb_fname)
{
	NTSTATUS status = NT_STATUS_OK;
	bool delete_on_close;
	uint32_t dosmode = 0;

	if (total_data < 1) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (fsp == NULL) {
		return NT_STATUS_INVALID_HANDLE;
	}

	delete_on_close = (CVAL(pdata,0) ? True : False);
	dosmode = fdos_mode(fsp);

	DEBUG(10,("smb_set_file_disposition_info: file %s, dosmode = %u, "
		"delete_on_close = %u\n",
		smb_fname_str_dbg(smb_fname),
		(unsigned int)dosmode,
		(unsigned int)delete_on_close ));

	if (delete_on_close) {
		status = can_set_delete_on_close(fsp, dosmode);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
	}

	/* The set is across all open files on this dev/inode pair. */
	if (!set_delete_on_close(fsp, delete_on_close,
				 conn->session_info->security_token,
				 conn->session_info->unix_token)) {
		return NT_STATUS_ACCESS_DENIED;
	}
	return NT_STATUS_OK;
}
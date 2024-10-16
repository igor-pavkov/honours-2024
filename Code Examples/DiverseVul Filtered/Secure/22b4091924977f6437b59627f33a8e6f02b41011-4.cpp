static NTSTATUS smb_set_info_standard(connection_struct *conn,
					const char *pdata,
					int total_data,
					files_struct *fsp,
					struct smb_filename *smb_fname)
{
	NTSTATUS status;
	struct smb_file_time ft;

	init_smb_file_time(&ft);

	if (total_data < 12) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (fsp == NULL) {
		return NT_STATUS_INVALID_HANDLE;
	}

	/* create time */
	ft.create_time = time_t_to_full_timespec(srv_make_unix_date2(pdata));
	/* access time */
	ft.atime = time_t_to_full_timespec(srv_make_unix_date2(pdata+4));
	/* write time */
	ft.mtime = time_t_to_full_timespec(srv_make_unix_date2(pdata+8));

	DEBUG(10,("smb_set_info_standard: file %s\n",
		smb_fname_str_dbg(smb_fname)));

	status = check_access_fsp(fsp, FILE_WRITE_ATTRIBUTES);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	status = smb_set_file_time(conn, fsp, smb_fname, &ft, true);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (fsp->fsp_flags.modified) {
		trigger_write_time_update_immediate(fsp);
	}
	return NT_STATUS_OK;
}
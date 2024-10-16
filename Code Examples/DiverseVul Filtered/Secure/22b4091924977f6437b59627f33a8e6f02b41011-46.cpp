static NTSTATUS smb_set_file_full_ea_info(connection_struct *conn,
				const char *pdata,
				int total_data,
				files_struct *fsp)
{
	struct ea_list *ea_list = NULL;
	NTSTATUS status;

	if (fsp == NULL) {
		return NT_STATUS_INVALID_HANDLE;
	}

	if (!lp_ea_support(SNUM(conn))) {
		DEBUG(10, ("smb_set_file_full_ea_info - ea_len = %u but "
			"EA's not supported.\n",
			(unsigned int)total_data));
		return NT_STATUS_EAS_NOT_SUPPORTED;
	}

	if (total_data < 10) {
		DEBUG(10, ("smb_set_file_full_ea_info - ea_len = %u "
			"too small.\n",
			(unsigned int)total_data));
		return NT_STATUS_INVALID_PARAMETER;
	}

	ea_list = read_nttrans_ea_list(talloc_tos(),
				pdata,
				total_data);

	if (!ea_list) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	status = set_ea(conn, fsp, ea_list);

	DEBUG(10, ("smb_set_file_full_ea_info on file %s returned %s\n",
		smb_fname_str_dbg(fsp->fsp_name),
		nt_errstr(status) ));

	return status;
}
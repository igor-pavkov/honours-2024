static NTSTATUS smb_file_mode_information(connection_struct *conn,
				const char *pdata,
				int total_data)
{
	uint32_t mode;

	if (total_data < 4) {
		return NT_STATUS_INVALID_PARAMETER;
	}
	mode = IVAL(pdata,0);
	if (mode != 0 && mode != 2 && mode != 4 && mode != 6) {
		return NT_STATUS_INVALID_PARAMETER;
	}
	return NT_STATUS_OK;
}
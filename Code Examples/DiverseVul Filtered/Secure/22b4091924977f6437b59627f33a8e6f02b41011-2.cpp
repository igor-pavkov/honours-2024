static NTSTATUS smb_file_position_information(connection_struct *conn,
				const char *pdata,
				int total_data,
				files_struct *fsp)
{
	uint64_t position_information;

	if (total_data < 8) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	if (fsp == NULL) {
		/* Ignore on pathname based set. */
		return NT_STATUS_OK;
	}

	position_information = (uint64_t)IVAL(pdata,0);
	position_information |= (((uint64_t)IVAL(pdata,4)) << 32);

	DEBUG(10,("smb_file_position_information: Set file position "
		  "information for file %s to %.0f\n", fsp_str_dbg(fsp),
		  (double)position_information));
	fh_set_position_information(fsp->fh, position_information);
	return NT_STATUS_OK;
}
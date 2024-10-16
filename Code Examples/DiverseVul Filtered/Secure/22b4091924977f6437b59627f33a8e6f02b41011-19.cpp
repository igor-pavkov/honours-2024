static NTSTATUS smb_set_file_end_of_file_info(connection_struct *conn,
					      struct smb_request *req,
					const char *pdata,
					int total_data,
					files_struct *fsp,
					struct smb_filename *smb_fname,
					bool fail_after_createfile)
{
	off_t size;

	if (total_data < 8) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	size = IVAL(pdata,0);
	size |= (((off_t)IVAL(pdata,4)) << 32);
	DEBUG(10,("smb_set_file_end_of_file_info: Set end of file info for "
		  "file %s to %.0f\n", smb_fname_str_dbg(smb_fname),
		  (double)size));

	return smb_set_file_size(conn, req,
				fsp,
				smb_fname,
				&smb_fname->st,
				size,
				fail_after_createfile);
}
NTSTATUS smbd_do_setfilepathinfo(connection_struct *conn,
				struct smb_request *req,
				TALLOC_CTX *mem_ctx,
				uint16_t info_level,
				files_struct *fsp,
				struct smb_filename *smb_fname,
				char **ppdata, int total_data,
				int *ret_data_size)
{
	char *pdata = *ppdata;
	NTSTATUS status = NT_STATUS_OK;
	int data_return_size = 0;

	if (INFO_LEVEL_IS_UNIX(info_level)) {
		if (!lp_unix_extensions()) {
			return NT_STATUS_INVALID_LEVEL;
		}

		status = smbd_do_posix_setfilepathinfo(conn,
						       req,
						       req,
						       info_level,
						       smb_fname,
						       fsp,
						       ppdata,
						       total_data,
						       &data_return_size);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
		*ret_data_size = data_return_size;
		return NT_STATUS_OK;
	}

	*ret_data_size = 0;

	DEBUG(3,("smbd_do_setfilepathinfo: %s (%s) info_level=%d "
		 "totdata=%d\n", smb_fname_str_dbg(smb_fname),
		 fsp_fnum_dbg(fsp),
		 info_level, total_data));

	switch (info_level) {

		case SMB_INFO_STANDARD:
		{
			status = smb_set_info_standard(conn,
					pdata,
					total_data,
					fsp,
					smb_fname);
			break;
		}

		case SMB_INFO_SET_EA:
		{
			status = smb_info_set_ea(conn,
						pdata,
						total_data,
						fsp,
						smb_fname);
			break;
		}

		case SMB_SET_FILE_BASIC_INFO:
		case SMB_FILE_BASIC_INFORMATION:
		{
			status = smb_set_file_basic_info(conn,
							pdata,
							total_data,
							fsp,
							smb_fname);
			break;
		}

		case SMB_FILE_ALLOCATION_INFORMATION:
		case SMB_SET_FILE_ALLOCATION_INFO:
		{
			status = smb_set_file_allocation_info(conn, req,
								pdata,
								total_data,
								fsp,
								smb_fname);
			break;
		}

		case SMB_FILE_END_OF_FILE_INFORMATION:
		case SMB_SET_FILE_END_OF_FILE_INFO:
		{
			/*
			 * XP/Win7 both fail after the createfile with
			 * SMB_SET_FILE_END_OF_FILE_INFO but not
			 * SMB_FILE_END_OF_FILE_INFORMATION (pass-through).
			 * The level is known here, so pass it down
			 * appropriately.
			 */
			bool should_fail =
			    (info_level == SMB_SET_FILE_END_OF_FILE_INFO);

			status = smb_set_file_end_of_file_info(conn, req,
								pdata,
								total_data,
								fsp,
								smb_fname,
								should_fail);
			break;
		}

		case SMB_FILE_DISPOSITION_INFORMATION:
		case SMB_SET_FILE_DISPOSITION_INFO: /* Set delete on close for open file. */
		{
#if 0
			/* JRA - We used to just ignore this on a path ? 
			 * Shouldn't this be invalid level on a pathname
			 * based call ?
			 */
			if (tran_call != TRANSACT2_SETFILEINFO) {
				return ERROR_NT(NT_STATUS_INVALID_LEVEL);
			}
#endif
			status = smb_set_file_disposition_info(conn,
						pdata,
						total_data,
						fsp,
						smb_fname);
			break;
		}

		case SMB_FILE_POSITION_INFORMATION:
		{
			status = smb_file_position_information(conn,
						pdata,
						total_data,
						fsp);
			break;
		}

		case SMB_FILE_FULL_EA_INFORMATION:
		{
			status = smb_set_file_full_ea_info(conn,
						pdata,
						total_data,
						fsp);
			break;
		}

		/* From tridge Samba4 : 
		 * MODE_INFORMATION in setfileinfo (I have no
		 * idea what "mode information" on a file is - it takes a value of 0,
		 * 2, 4 or 6. What could it be?).
		 */

		case SMB_FILE_MODE_INFORMATION:
		{
			status = smb_file_mode_information(conn,
						pdata,
						total_data);
			break;
		}

		/* [MS-SMB2] 3.3.5.21.1 states we MUST fail with STATUS_NOT_SUPPORTED. */
		case SMB_FILE_VALID_DATA_LENGTH_INFORMATION:
		case SMB_FILE_SHORT_NAME_INFORMATION:
			return NT_STATUS_NOT_SUPPORTED;

		case SMB_FILE_RENAME_INFORMATION:
		{
			status = smb_file_rename_information(conn, req,
							     pdata, total_data,
							     fsp, smb_fname);
			break;
		}

		case SMB2_FILE_RENAME_INFORMATION_INTERNAL:
		{
			/* SMB2 rename information. */
			status = smb2_file_rename_information(conn, req,
							     pdata, total_data,
							     fsp, smb_fname);
			break;
		}

		case SMB_FILE_LINK_INFORMATION:
		{
			status = smb_file_link_information(conn, req,
							pdata, total_data,
							fsp, smb_fname);
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
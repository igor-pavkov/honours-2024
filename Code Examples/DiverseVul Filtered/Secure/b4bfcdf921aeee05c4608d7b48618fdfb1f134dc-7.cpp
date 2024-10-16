static void call_nt_transact_ioctl(connection_struct *conn,
				   struct smb_request *req,
				   uint16 **ppsetup, uint32 setup_count,
				   char **ppparams, uint32 parameter_count,
				   char **ppdata, uint32 data_count,
				   uint32 max_data_count)
{
	NTSTATUS status;
	uint32 function;
	uint16 fidnum;
	files_struct *fsp;
	uint8 isFSctl;
	uint8 compfilter;
	char *out_data = NULL;
	uint32 out_data_len = 0;
	char *pdata = *ppdata;
	TALLOC_CTX *ctx = talloc_tos();

	if (setup_count != 8) {
		DEBUG(3,("call_nt_transact_ioctl: invalid setup count %d\n", setup_count));
		reply_nterror(req, NT_STATUS_NOT_SUPPORTED);
		return;
	}

	function = IVAL(*ppsetup, 0);
	fidnum = SVAL(*ppsetup, 4);
	isFSctl = CVAL(*ppsetup, 6);
	compfilter = CVAL(*ppsetup, 7);

	DEBUG(10, ("call_nt_transact_ioctl: function[0x%08X] FID[0x%04X] isFSctl[0x%02X] compfilter[0x%02X]\n", 
		 function, fidnum, isFSctl, compfilter));

	fsp=file_fsp(req, fidnum);

	/*
	 * We don't really implement IOCTLs, especially on files.
	 */
	if (!isFSctl) {
		DEBUG(10, ("isFSctl: 0x%02X indicates IOCTL, not FSCTL!\n",
			isFSctl));
		reply_nterror(req, NT_STATUS_NOT_SUPPORTED);
		return;
	}

	/* Has to be for an open file! */
	if (!check_fsp_open(conn, req, fsp)) {
		return;
	}

	SMB_PERFCOUNT_SET_IOCTL(&req->pcd, function);

	/*
	 * out_data might be allocated by the VFS module, but talloc should be
	 * used, and should be cleaned up when the request ends.
	 */
	status = SMB_VFS_FSCTL(fsp, 
			       ctx,
			       function, 
			       req->flags2,
			       (uint8_t *)pdata, 
			       data_count, 
			       (uint8_t **)&out_data,
			       max_data_count,
			       &out_data_len);
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
	} else {
		send_nt_replies(conn, req, NT_STATUS_OK, NULL, 0, out_data, out_data_len);
	}
}
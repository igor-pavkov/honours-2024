static void call_trans2qpipeinfo(connection_struct *conn,
				 struct smb_request *req,
				 unsigned int tran_call,
				 char **pparams, int total_params,
				 char **ppdata, int total_data,
				 unsigned int max_data_bytes)
{
	char *params = *pparams;
	char *pdata = *ppdata;
	unsigned int data_size = 0;
	unsigned int param_size = 2;
	uint16_t info_level;
	files_struct *fsp;

	if (!params) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	if (total_params < 4) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	fsp = file_fsp(req, SVAL(params,0));
	if (!fsp_is_np(fsp)) {
		reply_nterror(req, NT_STATUS_INVALID_HANDLE);
		return;
	}

	info_level = SVAL(params,2);

	*pparams = (char *)SMB_REALLOC(*pparams,2);
	if (*pparams == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}
	params = *pparams;
	SSVAL(params,0,0);
	if (max_data_bytes + DIR_ENTRY_SAFETY_MARGIN < max_data_bytes) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}
	data_size = max_data_bytes + DIR_ENTRY_SAFETY_MARGIN;
	*ppdata = (char *)SMB_REALLOC(*ppdata, data_size); 
	if (*ppdata == NULL ) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}
	pdata = *ppdata;

	switch (info_level) {
		case SMB_FILE_STANDARD_INFORMATION:
			memset(pdata,0,24);
			SOFF_T(pdata,0,4096LL);
			SIVAL(pdata,16,1);
			SIVAL(pdata,20,1);
			data_size = 24;
			break;

		default:
			reply_nterror(req, NT_STATUS_INVALID_LEVEL);
			return;
	}

	send_trans2_replies(conn, req, NT_STATUS_OK, params, param_size, *ppdata, data_size,
			    max_data_bytes);

	return;
}
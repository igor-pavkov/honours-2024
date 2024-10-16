static void call_nt_transact_set_security_desc(connection_struct *conn,
					       struct smb_request *req,
					       uint16 **ppsetup,
					       uint32 setup_count,
					       char **ppparams,
					       uint32 parameter_count,
					       char **ppdata,
					       uint32 data_count,
					       uint32 max_data_count)
{
	char *params= *ppparams;
	char *data = *ppdata;
	files_struct *fsp = NULL;
	uint32 security_info_sent = 0;
	NTSTATUS status;

	if(parameter_count < 8) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	if((fsp = file_fsp(req, SVAL(params,0))) == NULL) {
		reply_nterror(req, NT_STATUS_INVALID_HANDLE);
		return;
	}

	if (!CAN_WRITE(fsp->conn)) {
		reply_nterror(req, NT_STATUS_ACCESS_DENIED);
		return;
	}

	if(!lp_nt_acl_support(SNUM(conn))) {
		goto done;
	}

	security_info_sent = IVAL(params,4);

	DEBUG(3,("call_nt_transact_set_security_desc: file = %s, sent 0x%x\n",
		 fsp_str_dbg(fsp), (unsigned int)security_info_sent));

	if (data_count == 0) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	status = set_sd_blob(fsp, (uint8 *)data, data_count, security_info_sent);

	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		return;
	}

  done:
	send_nt_replies(conn, req, NT_STATUS_OK, NULL, 0, NULL, 0);
	return;
}
static void call_trans2qfsinfo(connection_struct *conn,
			       struct smb_request *req,
			       char **pparams, int total_params,
			       char **ppdata, int total_data,
			       unsigned int max_data_bytes)
{
	char *params = *pparams;
	uint16_t info_level;
	int data_len = 0;
	size_t fixed_portion;
	NTSTATUS status;

	if (total_params < 2) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	info_level = SVAL(params,0);

	if (ENCRYPTION_REQUIRED(conn) && !req->encrypted) {
		if (info_level != SMB_QUERY_CIFS_UNIX_INFO) {
			DEBUG(0,("call_trans2qfsinfo: encryption required "
				"and info level 0x%x sent.\n",
				(unsigned int)info_level));
			reply_nterror(req, NT_STATUS_ACCESS_DENIED);
			return;
		}
	}

	DEBUG(3,("call_trans2qfsinfo: level = %d\n", info_level));

	status = smbd_do_qfsinfo(req->xconn, conn, req,
				 info_level,
				 req->flags2,
				 max_data_bytes,
				 &fixed_portion,
				 NULL,
				 ppdata, &data_len);
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		return;
	}

	send_trans2_replies(conn, req, NT_STATUS_OK, params, 0, *ppdata, data_len,
			    max_data_bytes);

	DEBUG( 4, ( "%s info_level = %d\n",
		    smb_fn_name(req->cmd), info_level) );

	return;
}
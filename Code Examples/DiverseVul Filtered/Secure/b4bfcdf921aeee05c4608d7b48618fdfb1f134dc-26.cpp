static void call_nt_transact_query_security_desc(connection_struct *conn,
						 struct smb_request *req,
						 uint16 **ppsetup,
						 uint32 setup_count,
						 char **ppparams,
						 uint32 parameter_count,
						 char **ppdata,
						 uint32 data_count,
						 uint32 max_data_count)
{
	char *params = *ppparams;
	char *data = *ppdata;
	size_t sd_size = 0;
	uint32 security_info_wanted;
	files_struct *fsp = NULL;
	NTSTATUS status;
	uint8_t *marshalled_sd = NULL;

        if(parameter_count < 8) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	fsp = file_fsp(req, SVAL(params,0));
	if(!fsp) {
		reply_nterror(req, NT_STATUS_INVALID_HANDLE);
		return;
	}

	security_info_wanted = IVAL(params,4);

	DEBUG(3,("call_nt_transact_query_security_desc: file = %s, "
		 "info_wanted = 0x%x\n", fsp_str_dbg(fsp),
		 (unsigned int)security_info_wanted));

	params = nttrans_realloc(ppparams, 4);
	if(params == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}

	/*
	 * Get the permissions to return.
	 */

	status = smbd_do_query_security_desc(conn,
					talloc_tos(),
					fsp,
					security_info_wanted,
					max_data_count,
					&marshalled_sd,
					&sd_size);

	if (NT_STATUS_EQUAL(status, NT_STATUS_BUFFER_TOO_SMALL)) {
		SIVAL(params,0,(uint32_t)sd_size);
		send_nt_replies(conn, req, NT_STATUS_BUFFER_TOO_SMALL,
			params, 4, NULL, 0);
		return;
        }

	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		return;
	}

	SMB_ASSERT(sd_size > 0);

	SIVAL(params,0,(uint32_t)sd_size);

	if (max_data_count < sd_size) {
		send_nt_replies(conn, req, NT_STATUS_BUFFER_TOO_SMALL,
				params, 4, NULL, 0);
		return;
	}

	/*
	 * Allocate the data we will return.
	 */

	data = nttrans_realloc(ppdata, sd_size);
	if(data == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}

	memcpy(data, marshalled_sd, sd_size);

	send_nt_replies(conn, req, NT_STATUS_OK, params, 4, data, (int)sd_size);

	return;
}
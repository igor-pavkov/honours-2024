static void call_nt_transact_rename(connection_struct *conn,
				    struct smb_request *req,
				    uint16 **ppsetup, uint32 setup_count,
				    char **ppparams, uint32 parameter_count,
				    char **ppdata, uint32 data_count,
				    uint32 max_data_count)
{
	char *params = *ppparams;
	char *new_name = NULL;
	files_struct *fsp = NULL;
	bool dest_has_wcard = False;
	NTSTATUS status;
	TALLOC_CTX *ctx = talloc_tos();

        if(parameter_count < 5) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	fsp = file_fsp(req, SVAL(params, 0));
	if (!check_fsp(conn, req, fsp)) {
		return;
	}
	srvstr_get_path_wcard(ctx, params, req->flags2, &new_name, params+4,
			      parameter_count - 4,
			      STR_TERMINATE, &status, &dest_has_wcard);
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
		return;
	}

	/*
	 * W2K3 ignores this request as the RAW-RENAME test
	 * demonstrates, so we do.
	 */
	send_nt_replies(conn, req, NT_STATUS_OK, NULL, 0, NULL, 0);

	DEBUG(3,("nt transact rename from = %s, to = %s ignored!\n",
		 fsp_str_dbg(fsp), new_name));

	return;
}
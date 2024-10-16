static void call_trans2getdfsreferral(connection_struct *conn,
				      struct smb_request *req,
				      char **pparams, int total_params,
				      char **ppdata, int total_data,
				      unsigned int max_data_bytes)
{
	char *params = *pparams;
  	char *pathname = NULL;
	int reply_size = 0;
	int max_referral_level;
	NTSTATUS status = NT_STATUS_OK;
	TALLOC_CTX *ctx = talloc_tos();

	DEBUG(10,("call_trans2getdfsreferral\n"));

	if (total_params < 3) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	max_referral_level = SVAL(params,0);

	if(!lp_host_msdfs()) {
		reply_nterror(req, NT_STATUS_NOT_IMPLEMENTED);
		return;
	}

	srvstr_pull_talloc(ctx, params, req->flags2, &pathname, &params[2],
		    total_params - 2, STR_TERMINATE);
	if (!pathname) {
		reply_nterror(req, NT_STATUS_NOT_FOUND);
		return;
	}
	if((reply_size = setup_dfs_referral(conn, pathname, max_referral_level,
					    ppdata,&status)) < 0) {
		reply_nterror(req, status);
		return;
	}

	SSVAL((discard_const_p(uint8_t, req->inbuf)), smb_flg2,
	      SVAL(req->inbuf,smb_flg2) | FLAGS2_DFS_PATHNAMES);
	send_trans2_replies(conn, req, NT_STATUS_OK, 0,0,*ppdata,reply_size, max_data_bytes);

	return;
}
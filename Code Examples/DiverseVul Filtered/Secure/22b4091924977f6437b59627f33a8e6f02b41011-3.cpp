static void call_trans2ioctl(connection_struct *conn,
			     struct smb_request *req,
			     char **pparams, int total_params,
			     char **ppdata, int total_data,
			     unsigned int max_data_bytes)
{
	const struct loadparm_substitution *lp_sub =
		loadparm_s3_global_substitution();
	char *pdata = *ppdata;
	files_struct *fsp = file_fsp(req, SVAL(req->vwv+15, 0));
	NTSTATUS status;
	size_t len = 0;

	/* check for an invalid fid before proceeding */

	if (!fsp) {
		reply_nterror(req, NT_STATUS_INVALID_HANDLE);
		return;
	}

	if ((SVAL(req->vwv+16, 0) == LMCAT_SPL)
	    && (SVAL(req->vwv+17, 0) == LMFUNC_GETJOBID)) {
		*ppdata = (char *)SMB_REALLOC(*ppdata, 32);
		if (*ppdata == NULL) {
			reply_nterror(req, NT_STATUS_NO_MEMORY);
			return;
		}
		pdata = *ppdata;

		/* NOTE - THIS IS ASCII ONLY AT THE MOMENT - NOT SURE IF OS/2
			CAN ACCEPT THIS IN UNICODE. JRA. */

		/* Job number */
		SSVAL(pdata, 0, print_spool_rap_jobid(fsp->print_file));

		status = srvstr_push(pdata, req->flags2, pdata + 2,
			    lp_netbios_name(), 15,
			    STR_ASCII|STR_TERMINATE, &len); /* Our NetBIOS name */
		if (!NT_STATUS_IS_OK(status)) {
			reply_nterror(req, status);
			return;
		}
		status = srvstr_push(pdata, req->flags2, pdata+18,
			    lp_servicename(talloc_tos(), lp_sub, SNUM(conn)), 13,
			    STR_ASCII|STR_TERMINATE, &len); /* Service name */
		if (!NT_STATUS_IS_OK(status)) {
			reply_nterror(req, status);
			return;
		}
		send_trans2_replies(conn, req, NT_STATUS_OK, *pparams, 0, *ppdata, 32,
				    max_data_bytes);
		return;
	}

	DEBUG(2,("Unknown TRANS2_IOCTL\n"));
	reply_nterror(req, NT_STATUS_NOT_IMPLEMENTED);
}
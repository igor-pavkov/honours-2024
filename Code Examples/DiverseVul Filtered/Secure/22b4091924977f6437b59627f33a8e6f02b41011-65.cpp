static void call_trans2findnotifyfirst(connection_struct *conn,
				       struct smb_request *req,
				       char **pparams, int total_params,
				       char **ppdata, int total_data,
				       unsigned int max_data_bytes)
{
	char *params = *pparams;
	uint16_t info_level;

	if (total_params < 6) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	info_level = SVAL(params,4);
	DEBUG(3,("call_trans2findnotifyfirst - info_level %d\n", info_level));

	switch (info_level) {
		case 1:
		case 2:
			break;
		default:
			reply_nterror(req, NT_STATUS_INVALID_LEVEL);
			return;
	}

	/* Realloc the parameter and data sizes */
	*pparams = (char *)SMB_REALLOC(*pparams,6);
	if (*pparams == NULL) {
		reply_nterror(req, NT_STATUS_NO_MEMORY);
		return;
	}
	params = *pparams;

	SSVAL(params,0,fnf_handle);
	SSVAL(params,2,0); /* No changes */
	SSVAL(params,4,0); /* No EA errors */

	fnf_handle++;

	if(fnf_handle == 0)
		fnf_handle = 257;

	send_trans2_replies(conn, req, NT_STATUS_OK, params, 6, *ppdata, 0, max_data_bytes);

	return;
}
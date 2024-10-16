static void call_nt_transact_notify_change(connection_struct *conn,
					   struct smb_request *req,
					   uint16 **ppsetup,
					   uint32 setup_count,
					   char **ppparams,
					   uint32 parameter_count,
					   char **ppdata, uint32 data_count,
					   uint32 max_data_count,
					   uint32 max_param_count)
{
	uint16 *setup = *ppsetup;
	files_struct *fsp;
	uint32 filter;
	NTSTATUS status;
	bool recursive;

	if(setup_count < 6) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	fsp = file_fsp(req, SVAL(setup,4));
	filter = IVAL(setup, 0);
	recursive = (SVAL(setup, 6) != 0) ? True : False;

	DEBUG(3,("call_nt_transact_notify_change\n"));

	if(!fsp) {
		reply_nterror(req, NT_STATUS_INVALID_HANDLE);
		return;
	}

	{
		char *filter_string;

		if (!(filter_string = notify_filter_string(NULL, filter))) {
			reply_nterror(req,NT_STATUS_NO_MEMORY);
			return;
		}

		DEBUG(3,("call_nt_transact_notify_change: notify change "
			 "called on %s, filter = %s, recursive = %d\n",
			 fsp_str_dbg(fsp), filter_string, recursive));

		TALLOC_FREE(filter_string);
	}

	if((!fsp->is_directory) || (conn != fsp->conn)) {
		reply_nterror(req, NT_STATUS_INVALID_PARAMETER);
		return;
	}

	if (fsp->notify == NULL) {

		status = change_notify_create(fsp, filter, recursive);

		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(10, ("change_notify_create returned %s\n",
				   nt_errstr(status)));
			reply_nterror(req, status);
			return;
		}
	}

	if (change_notify_fsp_has_changes(fsp)) {

		/*
		 * We've got changes pending, respond immediately
		 */

		/*
		 * TODO: write a torture test to check the filtering behaviour
		 * here.
		 */

		change_notify_reply(req,
				    NT_STATUS_OK,
				    max_param_count,
				    fsp->notify,
				    smbd_smb1_notify_reply);

		/*
		 * change_notify_reply() above has independently sent its
		 * results
		 */
		return;
	}

	/*
	 * No changes pending, queue the request
	 */

	status = change_notify_add_request(req,
			max_param_count,
			filter,
			recursive, fsp,
			smbd_smb1_notify_reply);
	if (!NT_STATUS_IS_OK(status)) {
		reply_nterror(req, status);
	}
	return;
}
void remove_pending_change_notify_requests_by_fid(files_struct *fsp,
						  NTSTATUS status)
{
	if (fsp->notify == NULL) {
		return;
	}

	while (fsp->notify->requests != NULL) {
		change_notify_reply(fsp->notify->requests->req,
				    status, 0, NULL,
				    fsp->notify->requests->reply_fn);
		change_notify_remove_request(fsp->conn->sconn,
					     fsp->notify->requests);
	}
}
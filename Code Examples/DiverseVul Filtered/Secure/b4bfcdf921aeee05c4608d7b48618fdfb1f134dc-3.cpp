void reply_ntcancel(struct smb_request *req)
{
	/*
	 * Go through and cancel any pending change notifies.
	 */

	START_PROFILE(SMBntcancel);
	srv_cancel_sign_response(req->sconn);
	remove_pending_change_notify_requests_by_mid(req->sconn, req->mid);
	remove_pending_lock_requests_by_mid_smb1(req->sconn, req->mid);

	DEBUG(3,("reply_ntcancel: cancel called on mid = %llu.\n",
		(unsigned long long)req->mid));

	END_PROFILE(SMBntcancel);
	return;
}
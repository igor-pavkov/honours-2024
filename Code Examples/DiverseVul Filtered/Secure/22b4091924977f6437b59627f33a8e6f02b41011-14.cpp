static void smb_set_posix_lock_done(struct tevent_req *subreq)
{
	struct smb_request *req = NULL;
	NTSTATUS status;
	bool ok;

	ok = smbd_smb1_do_locks_extract_smbreq(subreq, talloc_tos(), &req);
	SMB_ASSERT(ok);

	status = smbd_smb1_do_locks_recv(subreq);
	TALLOC_FREE(subreq);

	if (NT_STATUS_IS_OK(status)) {
		char params[2] = {0};
		/* Fake up max_data_bytes here - we know it fits. */
		send_trans2_replies(
			req->conn,
			req,
			NT_STATUS_OK,
			params,
			2,
			NULL,
			0,
			0xffff);
	} else {
		reply_nterror(req, status);
		ok = srv_send_smb(
			req->xconn,
			(char *)req->outbuf,
			true,
			req->seqnum+1,
			IS_CONN_ENCRYPTED(req->conn),
			NULL);
		if (!ok) {
			exit_server_cleanly("smb_set_posix_lock_done: "
					    "srv_send_smb failed.");
		}
	}

	TALLOC_FREE(req);
	return;
}
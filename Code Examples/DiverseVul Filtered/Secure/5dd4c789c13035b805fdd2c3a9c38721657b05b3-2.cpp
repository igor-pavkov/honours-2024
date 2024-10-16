static void smbd_notify_cancel_by_map(struct notify_mid_map *map)
{
	struct smb_request *smbreq = map->req->req;
	struct smbd_server_connection *sconn = smbreq->sconn;
	struct smbd_smb2_request *smb2req = smbreq->smb2req;
	NTSTATUS notify_status = NT_STATUS_CANCELLED;

	if (smb2req != NULL) {
		NTSTATUS sstatus;

		if (smb2req->session == NULL) {
			sstatus = NT_STATUS_USER_SESSION_DELETED;
		} else {
			sstatus = smb2req->session->status;
		}

		if (NT_STATUS_EQUAL(sstatus, NT_STATUS_NETWORK_SESSION_EXPIRED)) {
			sstatus = NT_STATUS_OK;
		}

		if (!NT_STATUS_IS_OK(sstatus)) {
			notify_status = NT_STATUS_NOTIFY_CLEANUP;
		} else if (smb2req->tcon == NULL) {
			notify_status = NT_STATUS_NOTIFY_CLEANUP;
		} else if (!NT_STATUS_IS_OK(smb2req->tcon->status)) {
			notify_status = NT_STATUS_NOTIFY_CLEANUP;
		}
	}

	change_notify_reply(smbreq, notify_status,
			    0, NULL, map->req->reply_fn);
	change_notify_remove_request(sconn, map->req);
}
static void change_notify_remove_request(struct smbd_server_connection *sconn,
					 struct notify_change_request *remove_req)
{
	files_struct *fsp;
	struct notify_change_request *req;

	/*
	 * Paranoia checks, the fsp referenced must must have the request in
	 * its list of pending requests
	 */

	fsp = remove_req->fsp;
	SMB_ASSERT(fsp->notify != NULL);

	for (req = fsp->notify->requests; req; req = req->next) {
		if (req == remove_req) {
			break;
		}
	}

	if (req == NULL) {
		smb_panic("notify_req not found in fsp's requests");
	}

	DLIST_REMOVE(fsp->notify->requests, req);
	DLIST_REMOVE(sconn->smb1.notify_mid_maps, req->mid_map);
	TALLOC_FREE(req);
}
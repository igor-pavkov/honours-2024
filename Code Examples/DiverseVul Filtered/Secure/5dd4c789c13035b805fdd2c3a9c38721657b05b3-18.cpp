NTSTATUS change_notify_add_request(struct smb_request *req,
				uint32_t max_param,
				uint32_t filter, bool recursive,
				struct files_struct *fsp,
				void (*reply_fn)(struct smb_request *req,
					NTSTATUS error_code,
					uint8_t *buf, size_t len))
{
	struct notify_change_request *request = NULL;
	struct notify_mid_map *map = NULL;
	struct smbd_server_connection *sconn = req->sconn;

	DEBUG(10, ("change_notify_add_request: Adding request for %s: "
		   "max_param = %d\n", fsp_str_dbg(fsp), (int)max_param));

	if (!(request = talloc(NULL, struct notify_change_request))
	    || !(map = talloc(request, struct notify_mid_map))) {
		TALLOC_FREE(request);
		return NT_STATUS_NO_MEMORY;
	}

	request->mid_map = map;
	map->req = request;

	request->req = talloc_move(request, &req);
	request->max_param = max_param;
	request->filter = filter;
	request->fsp = fsp;
	request->reply_fn = reply_fn;
	request->backend_data = NULL;

	DLIST_ADD_END(fsp->notify->requests, request);

	map->mid = request->req->mid;
	DLIST_ADD(sconn->smb1.notify_mid_maps, map);

	return NT_STATUS_OK;
}
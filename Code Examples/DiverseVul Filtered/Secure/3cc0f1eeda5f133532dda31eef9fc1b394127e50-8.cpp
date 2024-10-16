NTSTATUS nbt_name_request_recv(struct nbt_name_request *req)
{
	if (!req) return NT_STATUS_NO_MEMORY;

	while (req->state < NBT_REQUEST_DONE) {
		if (tevent_loop_once(req->nbtsock->event_ctx) != 0) {
			req->state = NBT_REQUEST_ERROR;
			req->status = NT_STATUS_UNEXPECTED_NETWORK_ERROR;
			break;
		}
	}
	return req->status;
}
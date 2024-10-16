static int nbt_name_request_destructor(struct nbt_name_request *req)
{
	if (req->state == NBT_REQUEST_SEND) {
		DLIST_REMOVE(req->nbtsock->send_queue, req);
	}
	if (req->state == NBT_REQUEST_WAIT) {
		req->nbtsock->num_pending--;
	}
	if (req->name_trn_id != 0 && !req->is_reply) {
		idr_remove(req->nbtsock->idr, req->name_trn_id);
		req->name_trn_id = 0;
	}
	TALLOC_FREE(req->te);
	if (req->nbtsock->send_queue == NULL) {
		TEVENT_FD_NOT_WRITEABLE(req->nbtsock->fde);
	}
	if (req->nbtsock->num_pending == 0 &&
	    req->nbtsock->incoming.handler == NULL) {
		TEVENT_FD_NOT_READABLE(req->nbtsock->fde);
	}
	return 0;
}
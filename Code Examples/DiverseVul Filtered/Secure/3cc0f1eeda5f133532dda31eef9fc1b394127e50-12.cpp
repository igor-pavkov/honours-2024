static void nbt_name_socket_timeout(struct tevent_context *ev, struct tevent_timer *te,
				    struct timeval t, void *private_data)
{
	struct nbt_name_request *req = talloc_get_type(private_data,
						       struct nbt_name_request);

	if (req->num_retries != 0) {
		req->num_retries--;
		req->te = tevent_add_timer(req->nbtsock->event_ctx, req,
					   timeval_add(&t, req->timeout, 0),
					   nbt_name_socket_timeout, req);
		if (req->state != NBT_REQUEST_SEND) {
			req->state = NBT_REQUEST_SEND;
			DLIST_ADD_END(req->nbtsock->send_queue, req);
		}
		TEVENT_FD_WRITEABLE(req->nbtsock->fde);
		return;
	}

	nbt_name_request_destructor(req);
	if (req->num_replies == 0) {
		req->state = NBT_REQUEST_TIMEOUT;
		req->status = NT_STATUS_IO_TIMEOUT;
	} else {
		req->state = NBT_REQUEST_DONE;
		req->status = NT_STATUS_OK;
	}
	if (req->async.fn) {
		req->async.fn(req);
	} else if (req->is_reply) {
		talloc_free(req);
	}
}
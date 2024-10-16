static void nbt_name_socket_send(struct nbt_name_socket *nbtsock)
{
	struct nbt_name_request *req;
	TALLOC_CTX *tmp_ctx = talloc_new(nbtsock);
	NTSTATUS status;

	while ((req = nbtsock->send_queue)) {
		size_t len;

		len = req->encoded.length;
		status = socket_sendto(nbtsock->sock, &req->encoded, &len,
				       req->dest);
		if (NT_STATUS_IS_ERR(status)) goto failed;

		if (!NT_STATUS_IS_OK(status)) {
			talloc_free(tmp_ctx);
			return;
		}

		DLIST_REMOVE(nbtsock->send_queue, req);
		req->state = NBT_REQUEST_WAIT;
		if (req->is_reply) {
			talloc_free(req);
		} else {
			TEVENT_FD_READABLE(nbtsock->fde);
			nbtsock->num_pending++;
		}
	}

	TEVENT_FD_NOT_WRITEABLE(nbtsock->fde);
	talloc_free(tmp_ctx);
	return;

failed:
	DLIST_REMOVE(nbtsock->send_queue, req);
	nbt_name_request_destructor(req);
	req->status = status;
	req->state = NBT_REQUEST_ERROR;
	talloc_free(tmp_ctx);
	if (req->async.fn) {
		req->async.fn(req);
	} else if (req->is_reply) {
		talloc_free(req);
	}
	return;
}
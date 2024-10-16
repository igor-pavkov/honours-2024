struct nbt_name_request *nbt_name_request_send(TALLOC_CTX *mem_ctx,
					       struct nbt_name_socket *nbtsock,
					       struct socket_address *dest,
					       struct nbt_name_packet *request,
					       int timeout, int retries,
					       bool allow_multiple_replies)
{
	struct nbt_name_request *req;
	int id;
	enum ndr_err_code ndr_err;

	req = talloc_zero(mem_ctx, struct nbt_name_request);
	if (req == NULL) goto failed;

	req->nbtsock                = nbtsock;
	req->allow_multiple_replies = allow_multiple_replies;
	req->state                  = NBT_REQUEST_SEND;
	req->is_reply               = false;
	req->timeout                = timeout;
	req->num_retries            = retries;
	req->dest                   = socket_address_copy(req, dest);
	if (req->dest == NULL) goto failed;

	/* we select a random transaction id unless the user supplied one */
	if (request->name_trn_id == 0) {
		id = idr_get_new_random(req->nbtsock->idr, req, UINT16_MAX);
	} else {
		if (idr_find(req->nbtsock->idr, request->name_trn_id)) goto failed;
		id = idr_get_new_above(req->nbtsock->idr, req, request->name_trn_id,
				       UINT16_MAX);
	}
	if (id == -1) goto failed;

	request->name_trn_id = id;
	req->name_trn_id     = id;

	req->te = tevent_add_timer(nbtsock->event_ctx, req,
				   timeval_current_ofs(req->timeout, 0),
				   nbt_name_socket_timeout, req);

	talloc_set_destructor(req, nbt_name_request_destructor);

	ndr_err = ndr_push_struct_blob(&req->encoded, req,
				       request,
				       (ndr_push_flags_fn_t)ndr_push_nbt_name_packet);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) goto failed;

	DLIST_ADD_END(nbtsock->send_queue, req);

	if (DEBUGLVL(10)) {
		DEBUG(10,("Queueing nbt packet to %s:%d\n",
			  req->dest->addr, req->dest->port));
		NDR_PRINT_DEBUG(nbt_name_packet, request);
	}

	TEVENT_FD_WRITEABLE(nbtsock->fde);

	return req;

failed:
	talloc_free(req);
	return NULL;
}
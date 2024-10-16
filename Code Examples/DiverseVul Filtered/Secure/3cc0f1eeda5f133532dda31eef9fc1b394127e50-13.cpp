_PUBLIC_ NTSTATUS nbt_name_reply_send(struct nbt_name_socket *nbtsock,
			     struct socket_address *dest,
			     struct nbt_name_packet *request)
{
	struct nbt_name_request *req;
	enum ndr_err_code ndr_err;

	req = talloc_zero(nbtsock, struct nbt_name_request);
	NT_STATUS_HAVE_NO_MEMORY(req);

	req->nbtsock   = nbtsock;
	req->dest = socket_address_copy(req, dest);
	if (req->dest == NULL) goto failed;
	req->state     = NBT_REQUEST_SEND;
	req->is_reply = true;

	talloc_set_destructor(req, nbt_name_request_destructor);

	if (DEBUGLVL(10)) {
		NDR_PRINT_DEBUG(nbt_name_packet, request);
	}

	ndr_err = ndr_push_struct_blob(&req->encoded, req,
				       request,
				       (ndr_push_flags_fn_t)ndr_push_nbt_name_packet);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		talloc_free(req);
		return ndr_map_error2ntstatus(ndr_err);
	}

	DLIST_ADD_END(nbtsock->send_queue, req);

	TEVENT_FD_WRITEABLE(nbtsock->fde);

	return NT_STATUS_OK;

failed:
	talloc_free(req);
	return NT_STATUS_NO_MEMORY;
}
void nbt_name_socket_handle_response_packet(struct nbt_name_request *req,
					    struct nbt_name_packet *packet,
					    struct socket_address *src)
{
	/* if this is a WACK response, this we need to go back to waiting,
	   but perhaps increase the timeout */
	if ((packet->operation & NBT_OPCODE) == NBT_OPCODE_WACK) {
		uint32_t ttl;
		if (req->received_wack || packet->ancount < 1) {
			nbt_name_request_destructor(req);
			req->status = NT_STATUS_INVALID_NETWORK_RESPONSE;
			req->state  = NBT_REQUEST_ERROR;
			goto done;
		}
		talloc_free(req->te);
		/* we know we won't need any more retries - the server
		   has received our request */
		req->num_retries   = 0;
		req->received_wack = true;
		/*
		 * there is a timeout in the packet,
		 * it is 5 + 4 * num_old_addresses
		 *
		 * although w2k3 screws it up
		 * and uses num_old_addresses = 0
		 *
		 * so we better fallback to the maximum
		 * of num_old_addresses = 25 if we got
		 * a timeout of less than 9s (5 + 4*1)
		 * or more than 105s (5 + 4*25).
		 */
		ttl = packet->answers[0].ttl;
		if ((ttl < (5 + 4*1)) || (ttl > (5 + 4*25))) {
			ttl = 5 + 4*25;
		}
		req->timeout = ttl;
		req->te = tevent_add_timer(req->nbtsock->event_ctx, req,
					   timeval_current_ofs(req->timeout, 0),
					   nbt_name_socket_timeout, req);
		return;
	}


	req->replies = talloc_realloc(req, req->replies, struct nbt_name_reply, req->num_replies+1);
	if (req->replies == NULL) {
		nbt_name_request_destructor(req);
		req->state  = NBT_REQUEST_ERROR;
		req->status = NT_STATUS_NO_MEMORY;
		goto done;
	}

	talloc_steal(req, src);
	req->replies[req->num_replies].dest   = src;
	talloc_steal(req, packet);
	req->replies[req->num_replies].packet = packet;
	req->num_replies++;

	/* if we don't want multiple replies then we are done */
	if (req->allow_multiple_replies &&
	    req->num_replies < NBT_MAX_REPLIES) {
		return;
	}

	nbt_name_request_destructor(req);
	req->state  = NBT_REQUEST_DONE;
	req->status = NT_STATUS_OK;

done:
	if (req->async.fn) {
		req->async.fn(req);
	}
}
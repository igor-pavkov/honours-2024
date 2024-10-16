static WERROR dns_process_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx,
			       DATA_BLOB *out)
{
	struct dns_process_state *state = tevent_req_data(
		req, struct dns_process_state);
	enum ndr_err_code ndr_err;
	WERROR ret;

	if (tevent_req_is_werror(req, &ret)) {
		return ret;
	}
	if (state->dns_err != DNS_RCODE_OK) {
		goto drop;
	}
	state->out_packet.operation |= state->state.flags;

	if (state->state.sign) {
		ret = dns_sign_tsig(state->dns, mem_ctx, &state->state,
				    &state->out_packet, 0);
		if (!W_ERROR_IS_OK(ret)) {
			state->dns_err = DNS_RCODE_SERVFAIL;
			goto drop;
		}
	}

	if (DEBUGLVLC(DBGC_DNS, 8)) {
		NDR_PRINT_DEBUGC(DBGC_DNS, dns_name_packet, &state->out_packet);
	}

	ndr_err = ndr_push_struct_blob(
		out, mem_ctx, &state->out_packet,
		(ndr_push_flags_fn_t)ndr_push_dns_name_packet);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		DEBUG(1, ("Failed to push packet: %s!\n",
			  ndr_errstr(ndr_err)));
		state->dns_err = DNS_RCODE_SERVFAIL;
		goto drop;
	}
	return WERR_OK;

drop:
	*out = data_blob_talloc(mem_ctx, state->in->data, state->in->length);
	if (out->data == NULL) {
		return WERR_NOMEM;
	}
	out->data[2] |= 0x80; /* Toggle DNS_FLAG_REPLY */
	out->data[3] |= state->dns_err;
	return WERR_OK;
}
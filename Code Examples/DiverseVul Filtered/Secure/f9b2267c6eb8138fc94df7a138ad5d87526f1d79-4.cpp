static struct tevent_req *ldapsrv_process_call_send(TALLOC_CTX *mem_ctx,
						    struct tevent_context *ev,
						    struct tevent_queue *call_queue,
						    struct ldapsrv_call *call)
{
	struct tevent_req *req;
	struct ldapsrv_process_call_state *state;
	bool ok;

	req = tevent_req_create(mem_ctx, &state,
				struct ldapsrv_process_call_state);
	if (req == NULL) {
		return req;
	}

	state->call = call;

	ok = tevent_queue_add(call_queue, ev, req,
			      ldapsrv_process_call_trigger, NULL);
	if (!ok) {
		tevent_req_oom(req);
		return tevent_req_post(req, ev);
	}

	return req;
}
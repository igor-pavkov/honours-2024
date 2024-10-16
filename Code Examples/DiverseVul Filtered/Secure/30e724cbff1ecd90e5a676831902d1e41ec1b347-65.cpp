static void vfswrap_asys_finished(struct tevent_context *ev,
					struct tevent_fd *fde,
					uint16_t flags, void *p)
{
	struct asys_context *asys_ctx = (struct asys_context *)p;
	struct tevent_req *req;
	struct vfswrap_asys_state *state;
	int res;
	ssize_t ret;
	int err;
	void *private_data;

	if ((flags & TEVENT_FD_READ) == 0) {
		return;
	}

	while (true) {
		res = asys_result(asys_ctx, &ret, &err, &private_data);
		if (res == EINTR || res == EAGAIN) {
			return;
		}
#ifdef EWOULDBLOCK
		if (res == EWOULDBLOCK) {
			return;
		}
#endif

		if (res == ECANCELED) {
			return;
		}

		if (res != 0) {
			DEBUG(1, ("asys_result returned %s\n", strerror(res)));
			return;
		}

		req = talloc_get_type_abort(private_data, struct tevent_req);
		state = tevent_req_data(req, struct vfswrap_asys_state);

		talloc_set_destructor(state, NULL);

		state->ret = ret;
		state->err = err;
		tevent_req_defer_callback(req, ev);
		tevent_req_done(req);
	}
}
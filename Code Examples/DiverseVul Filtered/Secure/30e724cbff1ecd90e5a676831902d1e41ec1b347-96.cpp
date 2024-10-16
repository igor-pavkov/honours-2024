static ssize_t vfswrap_asys_ssize_t_recv(struct tevent_req *req, int *err)
{
	struct vfswrap_asys_state *state = tevent_req_data(
		req, struct vfswrap_asys_state);

	if (tevent_req_is_unix_error(req, err)) {
		return -1;
	}
	*err = state->err;
	return state->ret;
}
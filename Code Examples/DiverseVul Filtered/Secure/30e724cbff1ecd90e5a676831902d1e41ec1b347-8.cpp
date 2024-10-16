static struct tevent_req *vfswrap_fsync_send(struct vfs_handle_struct *handle,
					     TALLOC_CTX *mem_ctx,
					     struct tevent_context *ev,
					     struct files_struct *fsp)
{
	struct tevent_req *req;
	struct vfswrap_asys_state *state;
	int ret;

	req = tevent_req_create(mem_ctx, &state, struct vfswrap_asys_state);
	if (req == NULL) {
		return NULL;
	}
	if (!vfswrap_init_asys_ctx(handle->conn->sconn->conn)) {
		tevent_req_oom(req);
		return tevent_req_post(req, ev);
	}
	state->asys_ctx = handle->conn->sconn->conn->asys_ctx;
	state->req = req;

	ret = asys_fsync(state->asys_ctx, fsp->fh->fd, req);
	if (ret != 0) {
		tevent_req_error(req, ret);
		return tevent_req_post(req, ev);
	}
	talloc_set_destructor(state, vfswrap_asys_state_destructor);

	return req;
}
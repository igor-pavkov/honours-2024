static NTSTATUS vfswrap_copy_chunk_recv(struct vfs_handle_struct *handle,
					struct tevent_req *req,
					off_t *copied)
{
	struct vfs_cc_state *vfs_cc_state = tevent_req_data(req,
							struct vfs_cc_state);
	NTSTATUS status;

	if (tevent_req_is_nterror(req, &status)) {
		DEBUG(2, ("server side copy chunk failed: %s\n",
			  nt_errstr(status)));
		*copied = 0;
		tevent_req_received(req);
		return status;
	}

	*copied = vfs_cc_state->copied;
	DEBUG(10, ("server side copy chunk copied %lu\n",
		   (unsigned long)*copied));
	tevent_req_received(req);

	return NT_STATUS_OK;
}
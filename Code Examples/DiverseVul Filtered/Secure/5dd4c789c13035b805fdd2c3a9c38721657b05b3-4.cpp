static struct files_struct *notify_fsp_cb(struct files_struct *fsp,
					  void *private_data)
{
	struct notify_fsp_state *state = private_data;

	if (fsp == state->notified_fsp) {
		DBG_DEBUG("notify_callback called for %s\n", fsp_str_dbg(fsp));
		notify_fsp(fsp, state->when, state->e->action, state->e->path);
		return fsp;
	}

	return NULL;
}
void notify_callback(struct smbd_server_connection *sconn,
		     void *private_data, struct timespec when,
		     const struct notify_event *e)
{
	struct notify_fsp_state state = {
		.notified_fsp = private_data, .when = when, .e = e
	};
	files_forall(sconn, notify_fsp_cb, &state);
}
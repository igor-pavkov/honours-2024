void smbd_notifyd_restarted(struct messaging_context *msg,
			    void *private_data, uint32_t msg_type,
			    struct server_id server_id, DATA_BLOB *data)
{
	struct smbd_server_connection *sconn = talloc_get_type_abort(
		private_data, struct smbd_server_connection);

	TALLOC_FREE(sconn->notify_ctx);

	sconn->notify_ctx = notify_init(sconn, sconn->msg_ctx,
					sconn, notify_callback);
	if (sconn->notify_ctx == NULL) {
		DBG_DEBUG("notify_init failed\n");
		return;
	}

	files_forall(sconn, smbd_notifyd_reregister, sconn->notify_ctx);
}
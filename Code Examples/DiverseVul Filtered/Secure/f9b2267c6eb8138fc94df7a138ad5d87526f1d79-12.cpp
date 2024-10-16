static void ldapsrv_accept_priv(struct stream_connection *c)
{
	struct ldapsrv_service *ldapsrv_service = talloc_get_type_abort(
		c->private_data, struct ldapsrv_service);
	struct auth_session_info *session_info;

	session_info = system_session(ldapsrv_service->task->lp_ctx);
	if (!session_info) {
		stream_terminate_connection(c, "failed to setup system "
					    "session info");
		return;
	}
	ldapsrv_accept(c, session_info, true);
}
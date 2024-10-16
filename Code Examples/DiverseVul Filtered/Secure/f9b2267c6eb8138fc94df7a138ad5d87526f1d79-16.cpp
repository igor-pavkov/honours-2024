static void ldapsrv_accept_nonpriv(struct stream_connection *c)
{
	struct ldapsrv_service *ldapsrv_service = talloc_get_type_abort(
		c->private_data, struct ldapsrv_service);
	struct auth_session_info *session_info;
	NTSTATUS status;

	status = auth_anonymous_session_info(
		c, ldapsrv_service->task->lp_ctx, &session_info);
	if (!NT_STATUS_IS_OK(status)) {
		stream_terminate_connection(c, "failed to setup anonymous "
					    "session info");
		return;
	}
	ldapsrv_accept(c, session_info, false);
}
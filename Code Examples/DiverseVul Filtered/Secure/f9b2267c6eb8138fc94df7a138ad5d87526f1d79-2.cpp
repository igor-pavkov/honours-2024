static void ldapsrv_post_fork(struct task_server *task, struct process_details *pd)
{
	struct ldapsrv_service *ldap_service =
		talloc_get_type_abort(task->private_data, struct ldapsrv_service);

	ldap_service->sam_ctx = samdb_connect(ldap_service,
					      ldap_service->task->event_ctx,
					      ldap_service->task->lp_ctx,
					      system_session(ldap_service->task->lp_ctx),
					      NULL,
					      0);
	if (ldap_service->sam_ctx == NULL) {
		task_server_terminate(task, "Cannot open system session LDB",
				      true);
		return;
	}
}
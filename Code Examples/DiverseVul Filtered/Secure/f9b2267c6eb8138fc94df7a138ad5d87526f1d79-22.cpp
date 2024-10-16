NTSTATUS server_service_ldap_init(TALLOC_CTX *ctx)
{
	static const struct service_details details = {
		.inhibit_fork_on_accept = false,
		.inhibit_pre_fork = false,
		.task_init = ldapsrv_task_init,
		.post_fork = ldapsrv_post_fork,
	};
	return register_server_service(ctx, "ldap", &details);
}
NTSTATUS server_service_kdc_init(TALLOC_CTX *ctx)
{
	static const struct service_details details = {
		.inhibit_fork_on_accept = true,
		.inhibit_pre_fork = false,
		.task_init = kdc_task_init,
		.post_fork = kdc_post_fork
	};
	return register_server_service(ctx, "kdc", &details);
}
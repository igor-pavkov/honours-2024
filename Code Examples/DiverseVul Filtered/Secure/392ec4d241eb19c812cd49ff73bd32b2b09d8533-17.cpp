NTSTATUS server_service_dns_init(void)
{
	return register_server_service("dns", dns_task_init);
}
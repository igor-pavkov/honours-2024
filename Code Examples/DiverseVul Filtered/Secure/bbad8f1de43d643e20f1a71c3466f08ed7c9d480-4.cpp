static NTSTATUS kdc_task_init(struct task_server *task)
{
	struct kdc_server *kdc;
	NTSTATUS status;
	struct interface *ifaces;

	switch (lpcfg_server_role(task->lp_ctx)) {
	case ROLE_STANDALONE:
		task_server_terminate(task, "kdc: no KDC required in standalone configuration", false);
		return NT_STATUS_INVALID_DOMAIN_ROLE;
	case ROLE_DOMAIN_MEMBER:
		task_server_terminate(task, "kdc: no KDC required in member server configuration", false);
		return NT_STATUS_INVALID_DOMAIN_ROLE;
	case ROLE_DOMAIN_PDC:
	case ROLE_DOMAIN_BDC:
	case ROLE_IPA_DC:
		task_server_terminate(
		    task, "Cannot start KDC as a 'classic Samba' DC", false);
		return NT_STATUS_INVALID_DOMAIN_ROLE;
	case ROLE_ACTIVE_DIRECTORY_DC:
		/* Yes, we want a KDC */
		break;
	}

	load_interface_list(task, task->lp_ctx, &ifaces);

	if (iface_list_count(ifaces) == 0) {
		task_server_terminate(task, "kdc: no network interfaces configured", false);
		return NT_STATUS_UNSUCCESSFUL;
	}

	task_server_set_title(task, "task[kdc]");

	kdc = talloc_zero(task, struct kdc_server);
	if (kdc == NULL) {
		task_server_terminate(task, "kdc: out of memory", true);
		return NT_STATUS_NO_MEMORY;
	}

	kdc->task = task;
	task->private_data = kdc;

	/* start listening on the configured network interfaces */
	status = kdc_startup_interfaces(kdc, task->lp_ctx, ifaces,
					task->model_ops);
	if (!NT_STATUS_IS_OK(status)) {
		task_server_terminate(task, "kdc failed to setup interfaces", true);
		return status;
	}


	return NT_STATUS_OK;
}
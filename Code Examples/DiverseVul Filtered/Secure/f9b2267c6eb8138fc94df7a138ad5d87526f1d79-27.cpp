static NTSTATUS ldapsrv_task_init(struct task_server *task)
{	
	char *ldapi_path;
#ifdef WITH_LDAPI_PRIV_SOCKET
	char *priv_dir;
#endif
	const char *dns_host_name;
	struct ldapsrv_service *ldap_service;
	NTSTATUS status;

	switch (lpcfg_server_role(task->lp_ctx)) {
	case ROLE_STANDALONE:
		task_server_terminate(task, "ldap_server: no LDAP server required in standalone configuration", 
				      false);
		return NT_STATUS_INVALID_DOMAIN_ROLE;
	case ROLE_DOMAIN_MEMBER:
		task_server_terminate(task, "ldap_server: no LDAP server required in member server configuration", 
				      false);
		return NT_STATUS_INVALID_DOMAIN_ROLE;
	case ROLE_ACTIVE_DIRECTORY_DC:
		/* Yes, we want an LDAP server */
		break;
	}

	task_server_set_title(task, "task[ldapsrv]");

	ldap_service = talloc_zero(task, struct ldapsrv_service);
	if (ldap_service == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto failed;
	}

	ldap_service->task = task;

	dns_host_name = talloc_asprintf(ldap_service, "%s.%s",
					lpcfg_netbios_name(task->lp_ctx),
					lpcfg_dnsdomain(task->lp_ctx));
	if (dns_host_name == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto failed;
	}

	status = tstream_tls_params_server(ldap_service,
					   dns_host_name,
					   lpcfg_tls_enabled(task->lp_ctx),
					   lpcfg_tls_keyfile(ldap_service, task->lp_ctx),
					   lpcfg_tls_certfile(ldap_service, task->lp_ctx),
					   lpcfg_tls_cafile(ldap_service, task->lp_ctx),
					   lpcfg_tls_crlfile(ldap_service, task->lp_ctx),
					   lpcfg_tls_dhpfile(ldap_service, task->lp_ctx),
					   lpcfg_tls_priority(task->lp_ctx),
					   &ldap_service->tls_params);
	if (!NT_STATUS_IS_OK(status)) {
		DBG_ERR("ldapsrv failed tstream_tls_params_server - %s\n",
			nt_errstr(status));
		goto failed;
	}

	ldap_service->call_queue = tevent_queue_create(ldap_service, "ldapsrv_call_queue");
	if (ldap_service->call_queue == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto failed;
	}

	if (lpcfg_interfaces(task->lp_ctx) && lpcfg_bind_interfaces_only(task->lp_ctx)) {
		struct interface *ifaces;
		int num_interfaces;
		int i;

		load_interface_list(task, task->lp_ctx, &ifaces);
		num_interfaces = iface_list_count(ifaces);

		/* We have been given an interfaces line, and been 
		   told to only bind to those interfaces. Create a
		   socket per interface and bind to only these.
		*/
		for(i = 0; i < num_interfaces; i++) {
			const char *address = iface_list_n_ip(ifaces, i);
			status = add_socket(task, task->lp_ctx, task->model_ops,
					    address, ldap_service);
			if (!NT_STATUS_IS_OK(status)) goto failed;
		}
	} else {
		char **wcard;
		size_t i;
		size_t num_binds = 0;
		wcard = iface_list_wildcard(task);
		if (wcard == NULL) {
			DBG_ERR("No wildcard addresses available\n");
			status = NT_STATUS_UNSUCCESSFUL;
			goto failed;
		}
		for (i=0; wcard[i]; i++) {
			status = add_socket(task, task->lp_ctx, task->model_ops,
					    wcard[i], ldap_service);
			if (NT_STATUS_IS_OK(status)) {
				num_binds++;
			}
		}
		talloc_free(wcard);
		if (num_binds == 0) {
			status = NT_STATUS_UNSUCCESSFUL;
			goto failed;
		}
	}

	ldapi_path = lpcfg_private_path(ldap_service, task->lp_ctx, "ldapi");
	if (!ldapi_path) {
		status = NT_STATUS_UNSUCCESSFUL;
		goto failed;
	}

	status = stream_setup_socket(task, task->event_ctx, task->lp_ctx,
				     task->model_ops, &ldap_stream_nonpriv_ops,
				     "unix", ldapi_path, NULL, 
				     lpcfg_socket_options(task->lp_ctx),
				     ldap_service, task->process_context);
	talloc_free(ldapi_path);
	if (!NT_STATUS_IS_OK(status)) {
		DBG_ERR("ldapsrv failed to bind to %s - %s\n",
			ldapi_path, nt_errstr(status));
	}

#ifdef WITH_LDAPI_PRIV_SOCKET
	priv_dir = lpcfg_private_path(ldap_service, task->lp_ctx, "ldap_priv");
	if (priv_dir == NULL) {
		status = NT_STATUS_UNSUCCESSFUL;
		goto failed;
	}
	/*
	 * Make sure the directory for the privileged ldapi socket exists, and
	 * is of the correct permissions
	 */
	if (!directory_create_or_exist(priv_dir, 0750)) {
		task_server_terminate(task, "Cannot create ldap "
				      "privileged ldapi directory", true);
		return NT_STATUS_UNSUCCESSFUL;
	}
	ldapi_path = talloc_asprintf(ldap_service, "%s/ldapi", priv_dir);
	talloc_free(priv_dir);
	if (ldapi_path == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto failed;
	}

	status = stream_setup_socket(task, task->event_ctx, task->lp_ctx,
				     task->model_ops, &ldap_stream_priv_ops,
				     "unix", ldapi_path, NULL,
				     lpcfg_socket_options(task->lp_ctx),
				     ldap_service,
				     task->process_context);
	talloc_free(ldapi_path);
	if (!NT_STATUS_IS_OK(status)) {
		DBG_ERR("ldapsrv failed to bind to %s - %s\n",
			ldapi_path, nt_errstr(status));
	}

#endif

	/* register the server */
	irpc_add_name(task->msg_ctx, "ldap_server");

	task->private_data = ldap_service;

	return NT_STATUS_OK;

failed:
	task_server_terminate(task, "Failed to startup ldap server task", true);
	return status;
}
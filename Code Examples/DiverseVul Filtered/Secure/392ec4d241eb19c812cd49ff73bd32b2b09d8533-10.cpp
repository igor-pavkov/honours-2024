static void dns_task_init(struct task_server *task)
{
	struct dns_server *dns;
	NTSTATUS status;
	struct interface *ifaces = NULL;
	int ret;
	struct ldb_result *res;
	static const char * const attrs[] = { "name", NULL};
	static const char * const attrs_none[] = { NULL};
	unsigned int i;
	struct ldb_message *dns_acc;
	char *hostname_lower;
	char *dns_spn;

	switch (lpcfg_server_role(task->lp_ctx)) {
	case ROLE_STANDALONE:
		task_server_terminate(task, "dns: no DNS required in standalone configuration", false);
		return;
	case ROLE_DOMAIN_MEMBER:
		task_server_terminate(task, "dns: no DNS required in member server configuration", false);
		return;
	case ROLE_ACTIVE_DIRECTORY_DC:
		/* Yes, we want a DNS */
		break;
	}

	if (lpcfg_interfaces(task->lp_ctx) && lpcfg_bind_interfaces_only(task->lp_ctx)) {
		load_interface_list(task, task->lp_ctx, &ifaces);

		if (iface_list_count(ifaces) == 0) {
			task_server_terminate(task, "dns: no network interfaces configured", false);
			return;
		}
	}

	task_server_set_title(task, "task[dns]");

	dns = talloc_zero(task, struct dns_server);
	if (dns == NULL) {
		task_server_terminate(task, "dns: out of memory", true);
		return;
	}

	dns->task = task;
	/*FIXME: Make this a configurable option */
	dns->max_payload = 4096;

	dns->server_credentials = cli_credentials_init(dns);
	if (!dns->server_credentials) {
		task_server_terminate(task, "Failed to init server credentials\n", true);
		return;
	}

	dns->samdb = samdb_connect(dns, dns->task->event_ctx, dns->task->lp_ctx,
			      system_session(dns->task->lp_ctx), 0);
	if (!dns->samdb) {
		task_server_terminate(task, "dns: samdb_connect failed", true);
		return;
	}

	cli_credentials_set_conf(dns->server_credentials, task->lp_ctx);

	hostname_lower = strlower_talloc(dns, lpcfg_netbios_name(task->lp_ctx));
	dns_spn = talloc_asprintf(dns, "DNS/%s.%s",
				  hostname_lower,
				  lpcfg_dnsdomain(task->lp_ctx));
	TALLOC_FREE(hostname_lower);

	ret = dsdb_search_one(dns->samdb, dns, &dns_acc,
			      ldb_get_default_basedn(dns->samdb), LDB_SCOPE_SUBTREE,
			      attrs_none, 0, "(servicePrincipalName=%s)",
			      dns_spn);
	if (ret == LDB_SUCCESS) {
		TALLOC_FREE(dns_acc);
		if (!dns_spn) {
			task_server_terminate(task, "dns: talloc_asprintf failed", true);
			return;
		}
		status = cli_credentials_set_stored_principal(dns->server_credentials, task->lp_ctx, dns_spn);
		if (!NT_STATUS_IS_OK(status)) {
			task_server_terminate(task,
					      talloc_asprintf(task, "Failed to obtain server credentials for DNS, "
							      "despite finding it in the samdb! %s\n",
							      nt_errstr(status)),
					      true);
			return;
		}
	} else {
		TALLOC_FREE(dns_spn);
		status = cli_credentials_set_machine_account(dns->server_credentials, task->lp_ctx);
		if (!NT_STATUS_IS_OK(status)) {
			task_server_terminate(task,
					      talloc_asprintf(task, "Failed to obtain server credentials, perhaps a standalone server?: %s\n",
							      nt_errstr(status)),
					      true);
			return;
		}
	}

	dns->tkeys = tkey_store_init(dns, TKEY_BUFFER_SIZE);
	if (!dns->tkeys) {
		task_server_terminate(task, "Failed to allocate tkey storage\n", true);
		return;
	}

	// TODO: this search does not work against windows
	ret = dsdb_search(dns->samdb, dns, &res, NULL, LDB_SCOPE_SUBTREE,
			  attrs, DSDB_SEARCH_SEARCH_ALL_PARTITIONS, "(objectClass=dnsZone)");
	if (ret != LDB_SUCCESS) {
		task_server_terminate(task,
				      "dns: failed to look up root DNS zones",
				      true);
		return;
	}

	TYPESAFE_QSORT(res->msgs, res->count, dns_server_sort_zones);

	for (i=0; i < res->count; i++) {
		struct dns_server_zone *z;

		z = talloc_zero(dns, struct dns_server_zone);
		if (z == NULL) {
			task_server_terminate(task, "dns failed to allocate memory", true);
		}

		z->name = ldb_msg_find_attr_as_string(res->msgs[i], "name", NULL);
		z->dn = talloc_move(z, &res->msgs[i]->dn);
		/*
		 * Ignore the RootDNSServers zone and zones that we don't support yet
		 * RootDNSServers should never be returned (Windows DNS server don't)
		 * ..TrustAnchors should never be returned as is, (Windows returns
		 * TrustAnchors) and for the moment we don't support DNSSEC so we'd better
		 * not return this zone.
		 */
		if ((strcmp(z->name, "RootDNSServers") == 0) ||
		    (strcmp(z->name, "..TrustAnchors") == 0))
		{
			DEBUG(10, ("Ignoring zone %s\n", z->name));
			talloc_free(z);
			continue;
		}
		DLIST_ADD_END(dns->zones, z, NULL);
	}

	status = dns_startup_interfaces(dns, ifaces);
	if (!NT_STATUS_IS_OK(status)) {
		task_server_terminate(task, "dns failed to setup interfaces", true);
		return;
	}
}
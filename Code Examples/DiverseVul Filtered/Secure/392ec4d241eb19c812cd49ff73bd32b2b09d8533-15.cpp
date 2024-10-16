static NTSTATUS dns_startup_interfaces(struct dns_server *dns,
				       struct interface *ifaces)
{
	const struct model_ops *model_ops;
	int num_interfaces;
	TALLOC_CTX *tmp_ctx = talloc_new(dns);
	NTSTATUS status;
	int i;

	/* within the dns task we want to be a single process, so
	   ask for the single process model ops and pass these to the
	   stream_setup_socket() call. */
	model_ops = process_model_startup("single");
	if (!model_ops) {
		DEBUG(0,("Can't find 'single' process model_ops\n"));
		return NT_STATUS_INTERNAL_ERROR;
	}

	if (ifaces != NULL) {
		num_interfaces = iface_list_count(ifaces);

		for (i=0; i<num_interfaces; i++) {
			const char *address = talloc_strdup(tmp_ctx,
							    iface_list_n_ip(ifaces, i));

			status = dns_add_socket(dns, model_ops, "dns", address,
						DNS_SERVICE_PORT);
			NT_STATUS_NOT_OK_RETURN(status);
		}
	} else {
		char **wcard;
		wcard = iface_list_wildcard(tmp_ctx);
		if (wcard == NULL) {
			DEBUG(0, ("No wildcard address available\n"));
			return NT_STATUS_INTERNAL_ERROR;
		}
		for (i = 0; wcard[i] != NULL; i++) {
			status = dns_add_socket(dns, model_ops, "dns", wcard[i],
						DNS_SERVICE_PORT);
			NT_STATUS_NOT_OK_RETURN(status);
		}
	}

	talloc_free(tmp_ctx);

	return NT_STATUS_OK;
}
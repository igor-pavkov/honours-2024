static NTSTATUS kdc_startup_interfaces(struct kdc_server *kdc,
				       struct loadparm_context *lp_ctx,
				       struct interface *ifaces,
				       const struct model_ops *model_ops)
{
	int num_interfaces;
	TALLOC_CTX *tmp_ctx = talloc_new(kdc);
	NTSTATUS status;
	int i;
	uint16_t kdc_port = lpcfg_krb5_port(lp_ctx);
	uint16_t kpasswd_port = lpcfg_kpasswd_port(lp_ctx);
	bool done_wildcard = false;

	num_interfaces = iface_list_count(ifaces);

	/* if we are allowing incoming packets from any address, then
	   we need to bind to the wildcard address */
	if (!lpcfg_bind_interfaces_only(lp_ctx)) {
		size_t num_binds = 0;
		char **wcard = iface_list_wildcard(kdc);
		NT_STATUS_HAVE_NO_MEMORY(wcard);
		for (i=0; wcard[i]; i++) {
			if (kdc_port) {
				status = kdc_add_socket(kdc, model_ops,
							"kdc", wcard[i], kdc_port,
							kdc_process, false);
				if (NT_STATUS_IS_OK(status)) {
					num_binds++;
				}
			}

			if (kpasswd_port) {
				status = kdc_add_socket(kdc, model_ops,
							"kpasswd", wcard[i], kpasswd_port,
							kpasswd_process, false);
				if (NT_STATUS_IS_OK(status)) {
					num_binds++;
				}
			}
		}
		talloc_free(wcard);
		if (num_binds == 0) {
			return NT_STATUS_INVALID_PARAMETER_MIX;
		}
		done_wildcard = true;
	}

	for (i=0; i<num_interfaces; i++) {
		const char *address = talloc_strdup(tmp_ctx, iface_list_n_ip(ifaces, i));

		if (kdc_port) {
			status = kdc_add_socket(kdc, model_ops,
						"kdc", address, kdc_port,
						kdc_process, done_wildcard);
			NT_STATUS_NOT_OK_RETURN(status);
		}

		if (kpasswd_port) {
			status = kdc_add_socket(kdc, model_ops,
						"kpasswd", address, kpasswd_port,
						kpasswd_process, done_wildcard);
			NT_STATUS_NOT_OK_RETURN(status);
		}
	}

	talloc_free(tmp_ctx);

	return NT_STATUS_OK;
}
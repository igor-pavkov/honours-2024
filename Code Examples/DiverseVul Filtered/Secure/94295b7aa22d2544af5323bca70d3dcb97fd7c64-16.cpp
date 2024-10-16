static char *cli_session_setup_get_principal(
	TALLOC_CTX *mem_ctx, const char *spnego_principal,
	const char *remote_name, const char *dest_realm)
{
	char *principal = NULL;

	if (!lp_client_use_spnego_principal() ||
	    strequal(spnego_principal, ADS_IGNORE_PRINCIPAL)) {
		spnego_principal = NULL;
	}
	if (spnego_principal != NULL) {
		DEBUG(3, ("cli_session_setup_spnego: using spnego provided "
			  "principal %s\n", spnego_principal));
		return talloc_strdup(mem_ctx, spnego_principal);
	}
	if (is_ipaddress(remote_name) ||
	    strequal(remote_name, STAR_SMBSERVER)) {
		return NULL;
	}

	DEBUG(3, ("cli_session_setup_spnego: using target "
		  "hostname not SPNEGO principal\n"));

	if (dest_realm) {
		char *realm = strupper_talloc(talloc_tos(), dest_realm);
		if (realm == NULL) {
			return NULL;
		}
		principal = talloc_asprintf(talloc_tos(), "cifs/%s@%s",
					    remote_name, realm);
		TALLOC_FREE(realm);
	} else {
		principal = kerberos_get_principal_from_service_hostname(
			talloc_tos(), "cifs", remote_name, lp_realm());
	}
	DEBUG(3, ("cli_session_setup_spnego: guessed server principal=%s\n",
		  principal ? principal : "<null>"));

	return principal;
}
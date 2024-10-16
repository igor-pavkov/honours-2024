static int acl_validate_spn_value(TALLOC_CTX *mem_ctx,
				  struct ldb_context *ldb,
				  const char *spn_value,
				  uint32_t userAccountControl,
				  const char *samAccountName,
				  const char *dnsHostName,
				  const char *netbios_name,
				  const char *ntds_guid)
{
	int ret, princ_size;
	krb5_context krb_ctx;
	krb5_error_code kerr;
	krb5_principal principal;
	char *instanceName;
	char *serviceType;
	char *serviceName;
	const char *forest_name = samdb_forest_name(ldb, mem_ctx);
	const char *base_domain = samdb_default_domain_name(ldb, mem_ctx);
	struct loadparm_context *lp_ctx = talloc_get_type(ldb_get_opaque(ldb, "loadparm"),
							  struct loadparm_context);
	bool is_dc = (userAccountControl & UF_SERVER_TRUST_ACCOUNT) ||
		(userAccountControl & UF_PARTIAL_SECRETS_ACCOUNT);

	if (strcasecmp_m(spn_value, samAccountName) == 0) {
		/* MacOS X sets this value, and setting an SPN of your
		 * own samAccountName is both pointless and safe */
		return LDB_SUCCESS;
	}

	kerr = smb_krb5_init_context_basic(mem_ctx,
					   lp_ctx,
					   &krb_ctx);
	if (kerr != 0) {
		return ldb_error(ldb, LDB_ERR_OPERATIONS_ERROR,
				 "Could not initialize kerberos context.");
	}

	ret = krb5_parse_name(krb_ctx, spn_value, &principal);
	if (ret) {
		krb5_free_context(krb_ctx);
		return LDB_ERR_CONSTRAINT_VIOLATION;
	}

	princ_size = krb5_princ_size(krb_ctx, principal);
	if (princ_size < 2) {
		DBG_WARNING("princ_size=%d\n", princ_size);
		goto fail;
	}

	instanceName = smb_krb5_principal_get_comp_string(mem_ctx, krb_ctx,
							  principal, 1);
	serviceType = smb_krb5_principal_get_comp_string(mem_ctx, krb_ctx,
							 principal, 0);
	if (krb5_princ_size(krb_ctx, principal) == 3) {
		serviceName = smb_krb5_principal_get_comp_string(mem_ctx, krb_ctx,
								 principal, 2);
	} else {
		serviceName = NULL;
	}

	if (serviceName) {
		if (!is_dc) {
			DBG_WARNING("is_dc=false, serviceName=%s,"
				    "serviceType=%s\n", serviceName,
				  serviceType);
			goto fail;
		}
		if (strcasecmp(serviceType, "ldap") == 0) {
			if (strcasecmp(serviceName, netbios_name) != 0 &&
			    strcasecmp(serviceName, forest_name) != 0) {
				DBG_WARNING("serviceName=%s\n", serviceName);
				goto fail;
			}

		} else if (strcasecmp(serviceType, "gc") == 0) {
			if (strcasecmp(serviceName, forest_name) != 0) {
				DBG_WARNING("serviceName=%s\n", serviceName);
				goto fail;
			}
		} else {
			if (strcasecmp(serviceName, base_domain) != 0 &&
			    strcasecmp(serviceName, netbios_name) != 0) {
				DBG_WARNING("serviceType=%s, "
					    "serviceName=%s\n",
					    serviceType, serviceName);
				goto fail;
			}
		}
	}
	/* instanceName can be samAccountName without $ or dnsHostName
	 * or "ntds_guid._msdcs.forest_domain for DC objects */
	if (strlen(instanceName) == (strlen(samAccountName) - 1)
	    && strncasecmp(instanceName, samAccountName,
			   strlen(samAccountName) - 1) == 0) {
		goto success;
	}
	if ((dnsHostName != NULL) &&
	    (strcasecmp(instanceName, dnsHostName) == 0)) {
		goto success;
	}
	if (is_dc) {
		const char *guid_str;
		guid_str = talloc_asprintf(mem_ctx,"%s._msdcs.%s",
					   ntds_guid,
					   forest_name);
		if (strcasecmp(instanceName, guid_str) == 0) {
			goto success;
		}
	}

fail:
	krb5_free_principal(krb_ctx, principal);
	krb5_free_context(krb_ctx);
	ldb_debug_set(ldb, LDB_DEBUG_WARNING,
		      "acl: spn validation failed for "
		      "spn[%s] uac[0x%x] account[%s] hostname[%s] "
		      "nbname[%s] ntds[%s] forest[%s] domain[%s]\n",
		      spn_value, (unsigned)userAccountControl,
		      samAccountName, dnsHostName,
		      netbios_name, ntds_guid,
		      forest_name, base_domain);
	return LDB_ERR_CONSTRAINT_VIOLATION;

success:
	krb5_free_principal(krb_ctx, principal);
	krb5_free_context(krb_ctx);
	return LDB_SUCCESS;
}
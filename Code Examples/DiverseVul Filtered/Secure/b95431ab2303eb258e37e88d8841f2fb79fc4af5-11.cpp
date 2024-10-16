static int acl_check_dns_host_name(TALLOC_CTX *mem_ctx,
				   struct ldb_module *module,
				   struct ldb_request *req,
				   const struct ldb_message_element *el,
				   struct security_descriptor *sd,
				   struct dom_sid *sid,
				   const struct dsdb_attribute *attr,
				   const struct dsdb_class *objectclass)
{
	int ret;
	unsigned i;
	TALLOC_CTX *tmp_ctx = NULL;
	struct ldb_context *ldb = ldb_module_get_ctx(module);
	const struct dsdb_schema *schema = NULL;
	const struct ldb_message_element *allowed_suffixes = NULL;
	struct ldb_result *nc_res = NULL;
	struct ldb_dn *nc_root = NULL;
	const char *nc_dns_name = NULL;
	const char *dnsHostName_str = NULL;
	size_t dns_host_name_len;
	size_t account_name_len;
	const struct ldb_message *msg = NULL;
	const struct ldb_message *search_res = NULL;
	const struct ldb_val *samAccountName = NULL;
	const struct ldb_val *dnsHostName = NULL;
	const struct dsdb_class *computer_objectclass = NULL;
	bool is_subclass;

	static const char *nc_attrs[] = {
		"msDS-AllowedDNSSuffixes",
		NULL
	};

	if (el->num_values == 0) {
		return LDB_SUCCESS;
	}
	dnsHostName = &el->values[0];

	tmp_ctx = talloc_new(mem_ctx);
	if (tmp_ctx == NULL) {
		return ldb_oom(ldb);
	}

	/* if we have wp, we can do whatever we like */
	ret = acl_check_access_on_attribute(module,
					    tmp_ctx,
					    sd,
					    sid,
					    SEC_ADS_WRITE_PROP,
					    attr, objectclass);
	if (ret == LDB_SUCCESS) {
		talloc_free(tmp_ctx);
		return LDB_SUCCESS;
	}

	ret = acl_check_extended_right(tmp_ctx,
				       module,
				       req,
				       objectclass,
				       sd,
				       acl_user_token(module),
				       GUID_DRS_DNS_HOST_NAME,
				       SEC_ADS_SELF_WRITE,
				       sid);

	if (ret != LDB_SUCCESS) {
		dsdb_acl_debug(sd, acl_user_token(module),
			       req->op.mod.message->dn,
			       true,
			       10);
		talloc_free(tmp_ctx);
		return ret;
	}

	/*
	 * If we have "validated write dnshostname", allow delete of
	 * any existing value (this keeps constrained delete to the
	 * same rules as unconstrained)
	 */
	if (req->operation == LDB_MODIFY) {
		struct ldb_result *acl_res = NULL;

		static const char *acl_attrs[] = {
			"sAMAccountName",
			NULL
		};

		msg = req->op.mod.message;

		/*
		 * If not add or replace (eg delete),
		 * return success
		 */
		if ((el->flags
		     & (LDB_FLAG_MOD_ADD|LDB_FLAG_MOD_REPLACE)) == 0)
		{
			talloc_free(tmp_ctx);
			return LDB_SUCCESS;
		}

		ret = dsdb_module_search_dn(module, tmp_ctx,
					    &acl_res, msg->dn,
					    acl_attrs,
					    DSDB_FLAG_NEXT_MODULE |
					    DSDB_FLAG_AS_SYSTEM |
					    DSDB_SEARCH_SHOW_RECYCLED,
					    req);
		if (ret != LDB_SUCCESS) {
			talloc_free(tmp_ctx);
			return ret;
		}

		search_res = acl_res->msgs[0];
	} else if (req->operation == LDB_ADD) {
		msg = req->op.add.message;
		search_res = msg;
	} else {
		talloc_free(tmp_ctx);
		return LDB_ERR_OPERATIONS_ERROR;
	}

        /* Check if the account has objectclass 'computer' or 'server'. */

	schema = dsdb_get_schema(ldb, req);
	if (schema == NULL) {
		talloc_free(tmp_ctx);
		return ldb_operr(ldb);
	}

	computer_objectclass = dsdb_class_by_lDAPDisplayName(schema, "computer");
	if (computer_objectclass == NULL) {
		talloc_free(tmp_ctx);
		return ldb_operr(ldb);
	}

	is_subclass = dsdb_is_subclass_of(schema, objectclass, computer_objectclass);
	if (!is_subclass) {
		/* The account is not a computer -- check if it's a server. */

		const struct dsdb_class *server_objectclass = NULL;

		server_objectclass = dsdb_class_by_lDAPDisplayName(schema, "server");
		if (server_objectclass == NULL) {
			talloc_free(tmp_ctx);
			return ldb_operr(ldb);
		}

		is_subclass = dsdb_is_subclass_of(schema, objectclass, server_objectclass);
		if (!is_subclass) {
			/* Not a computer or server, so no need to validate. */
			talloc_free(tmp_ctx);
			return LDB_SUCCESS;
		}
	}

	samAccountName = ldb_msg_find_ldb_val(search_res, "sAMAccountName");

	ret = dsdb_msg_get_single_value(msg,
					"sAMAccountName",
					samAccountName,
					&samAccountName,
					req->operation);
	if (ret != LDB_SUCCESS) {
		talloc_free(tmp_ctx);
		return ret;
	}

	account_name_len = samAccountName->length;
	if (account_name_len && samAccountName->data[account_name_len - 1] == '$') {
		/* Account for the '$' character. */
		--account_name_len;
	}

	dnsHostName_str = (const char *)dnsHostName->data;
	dns_host_name_len = dnsHostName->length;

	/* Check that sAMAccountName matches the new dNSHostName. */

	if (dns_host_name_len < account_name_len) {
		goto fail;
	}
	if (strncasecmp(dnsHostName_str,
			(const char *)samAccountName->data,
			account_name_len) != 0)
	{
		goto fail;
	}

	dnsHostName_str += account_name_len;
	dns_host_name_len -= account_name_len;

	/* Check the '.' character */

	if (dns_host_name_len == 0 || *dnsHostName_str != '.') {
		goto fail;
	}

	++dnsHostName_str;
	--dns_host_name_len;

	/* Now we check the suffix. */

	ret = dsdb_find_nc_root(ldb,
				tmp_ctx,
				search_res->dn,
				&nc_root);
	if (ret != LDB_SUCCESS) {
		talloc_free(tmp_ctx);
		return ret;
	}

	nc_dns_name = samdb_dn_to_dns_domain(tmp_ctx, nc_root);
	if (nc_dns_name == NULL) {
		talloc_free(tmp_ctx);
		return ldb_operr(ldb);
	}

	if (strlen(nc_dns_name) == dns_host_name_len &&
	    strncasecmp(dnsHostName_str,
			nc_dns_name,
			dns_host_name_len) == 0)
	{
		/* It matches -- success. */
		talloc_free(tmp_ctx);
		return LDB_SUCCESS;
	}

	/* We didn't get a match, so now try msDS-AllowedDNSSuffixes. */

	ret = dsdb_module_search_dn(module, tmp_ctx,
				    &nc_res, nc_root,
				    nc_attrs,
				    DSDB_FLAG_NEXT_MODULE |
				    DSDB_FLAG_AS_SYSTEM |
				    DSDB_SEARCH_SHOW_RECYCLED,
				    req);
	if (ret != LDB_SUCCESS) {
		talloc_free(tmp_ctx);
		return ret;
	}

	allowed_suffixes = ldb_msg_find_element(nc_res->msgs[0],
						"msDS-AllowedDNSSuffixes");
	if (allowed_suffixes == NULL) {
		goto fail;
	}

	for (i = 0; i < allowed_suffixes->num_values; ++i) {
		const struct ldb_val *suffix = &allowed_suffixes->values[i];

		if (suffix->length == dns_host_name_len &&
		    strncasecmp(dnsHostName_str,
				(const char *)suffix->data,
				dns_host_name_len) == 0)
		{
			/* It matches -- success. */
			talloc_free(tmp_ctx);
			return LDB_SUCCESS;
		}
	}

fail:
	ldb_debug_set(ldb, LDB_DEBUG_WARNING,
		      "acl: hostname validation failed for "
		      "hostname[%.*s] account[%.*s]\n",
		      (int)dnsHostName->length, dnsHostName->data,
		      (int)samAccountName->length, samAccountName->data);
	talloc_free(tmp_ctx);
	return LDB_ERR_CONSTRAINT_VIOLATION;
}
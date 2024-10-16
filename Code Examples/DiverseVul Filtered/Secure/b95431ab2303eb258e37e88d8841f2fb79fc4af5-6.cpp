static int acl_check_spn(TALLOC_CTX *mem_ctx,
			 struct ldb_module *module,
			 struct ldb_request *req,
			 const struct ldb_message_element *el,
			 struct security_descriptor *sd,
			 struct dom_sid *sid,
			 const struct dsdb_attribute *attr,
			 const struct dsdb_class *objectclass)
{
	int ret;
	unsigned int i;
	TALLOC_CTX *tmp_ctx = talloc_new(mem_ctx);
	struct ldb_context *ldb = ldb_module_get_ctx(module);
	struct ldb_result *acl_res;
	struct ldb_result *netbios_res;
	struct ldb_dn *partitions_dn = samdb_partitions_dn(ldb, tmp_ctx);
	uint32_t userAccountControl;
	const char *samAccountName;
	const char *dnsHostName;
	const char *netbios_name;
	struct GUID ntds;
	char *ntds_guid = NULL;

	static const char *acl_attrs[] = {
		"samAccountName",
		"dnsHostName",
		"userAccountControl",
		NULL
	};
	static const char *netbios_attrs[] = {
		"nETBIOSName",
		NULL
	};

	/* if we have wp, we can do whatever we like */
	if (acl_check_access_on_attribute(module,
					  tmp_ctx,
					  sd,
					  sid,
					  SEC_ADS_WRITE_PROP,
					  attr, objectclass) == LDB_SUCCESS) {
		talloc_free(tmp_ctx);
		return LDB_SUCCESS;
	}

	ret = acl_check_extended_right(tmp_ctx,
				       module,
				       req,
				       objectclass,
				       sd,
				       acl_user_token(module),
				       GUID_DRS_VALIDATE_SPN,
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
	 * If we have "validated write spn", allow delete of any
	 * existing value (this keeps constrained delete to the same
	 * rules as unconstrained)
	 */
	if (req->operation == LDB_MODIFY) {
		/*
		 * If not add or replace (eg delete),
		 * return success
		 */
		if (LDB_FLAG_MOD_TYPE(el->flags) != LDB_FLAG_MOD_ADD &&
		    LDB_FLAG_MOD_TYPE(el->flags) != LDB_FLAG_MOD_REPLACE)
		{
			talloc_free(tmp_ctx);
			return LDB_SUCCESS;
		}
	}

	ret = dsdb_module_search_dn(module, tmp_ctx,
				    &acl_res, req->op.mod.message->dn,
				    acl_attrs,
				    DSDB_FLAG_NEXT_MODULE |
				    DSDB_FLAG_AS_SYSTEM |
				    DSDB_SEARCH_SHOW_RECYCLED,
				    req);
	if (ret != LDB_SUCCESS) {
		talloc_free(tmp_ctx);
		return ret;
	}

	userAccountControl = ldb_msg_find_attr_as_uint(acl_res->msgs[0], "userAccountControl", 0);
	dnsHostName = ldb_msg_find_attr_as_string(acl_res->msgs[0], "dnsHostName", NULL);
	samAccountName = ldb_msg_find_attr_as_string(acl_res->msgs[0], "samAccountName", NULL);

	ret = dsdb_module_search(module, tmp_ctx,
				 &netbios_res, partitions_dn,
				 LDB_SCOPE_ONELEVEL,
				 netbios_attrs,
				 DSDB_FLAG_NEXT_MODULE |
				 DSDB_FLAG_AS_SYSTEM,
				 req,
				 "(ncName=%s)",
				 ldb_dn_get_linearized(ldb_get_default_basedn(ldb)));

	netbios_name = ldb_msg_find_attr_as_string(netbios_res->msgs[0], "nETBIOSName", NULL);

	/* NTDSDSA objectGuid of object we are checking SPN for */
	if (userAccountControl & (UF_SERVER_TRUST_ACCOUNT | UF_PARTIAL_SECRETS_ACCOUNT)) {
		ret = dsdb_module_find_ntdsguid_for_computer(module, tmp_ctx,
							     req->op.mod.message->dn, &ntds, req);
		if (ret != LDB_SUCCESS) {
			ldb_asprintf_errstring(ldb, "Failed to find NTDSDSA objectGuid for %s: %s",
					       ldb_dn_get_linearized(req->op.mod.message->dn),
					       ldb_strerror(ret));
			talloc_free(tmp_ctx);
			return LDB_ERR_OPERATIONS_ERROR;
		}
		ntds_guid = GUID_string(tmp_ctx, &ntds);
	}

	for (i=0; i < el->num_values; i++) {
		ret = acl_validate_spn_value(tmp_ctx,
					     ldb,
					     (char *)el->values[i].data,
					     userAccountControl,
					     samAccountName,
					     dnsHostName,
					     netbios_name,
					     ntds_guid);
		if (ret != LDB_SUCCESS) {
			talloc_free(tmp_ctx);
			return ret;
		}
	}
	talloc_free(tmp_ctx);
	return LDB_SUCCESS;
}
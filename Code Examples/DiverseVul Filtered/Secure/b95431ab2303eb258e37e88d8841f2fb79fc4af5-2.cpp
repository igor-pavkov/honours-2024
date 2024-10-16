static int acl_check_reanimate_tombstone(TALLOC_CTX *mem_ctx,
					 struct ldb_module *module,
					 struct ldb_request *req,
					 struct ldb_dn *nc_root)
{
	int ret;
	struct ldb_result *acl_res;
	struct security_descriptor *sd = NULL;
	struct dom_sid *sid = NULL;
	const struct dsdb_schema *schema = NULL;
	const struct dsdb_class *objectclass = NULL;
	struct ldb_context *ldb = ldb_module_get_ctx(module);
	static const char *acl_attrs[] = {
		"nTSecurityDescriptor",
		"objectClass",
		"objectSid",
		NULL
	};

	ret = dsdb_module_search_dn(module, mem_ctx, &acl_res,
				    nc_root, acl_attrs,
				    DSDB_FLAG_NEXT_MODULE |
				    DSDB_FLAG_AS_SYSTEM |
				    DSDB_SEARCH_SHOW_RECYCLED, req);
	if (ret != LDB_SUCCESS) {
		DEBUG(10,("acl: failed to find object %s\n",
			  ldb_dn_get_linearized(nc_root)));
		return ret;
	}

	ret = dsdb_get_sd_from_ldb_message(mem_ctx, req, acl_res->msgs[0], &sd);
	sid = samdb_result_dom_sid(mem_ctx, acl_res->msgs[0], "objectSid");
	schema = dsdb_get_schema(ldb, req);
	if (!schema) {
		return LDB_ERR_OPERATIONS_ERROR;
	}
	objectclass = dsdb_get_structural_oc_from_msg(schema, acl_res->msgs[0]);
	if (ret != LDB_SUCCESS || !sd) {
		return ldb_operr(ldb_module_get_ctx(module));
	}
	return acl_check_extended_right(mem_ctx,
					module,
					req,
					objectclass,
					sd,
					acl_user_token(module),
					GUID_DRS_REANIMATE_TOMBSTONE,
					SEC_ADS_CONTROL_ACCESS, sid);
}
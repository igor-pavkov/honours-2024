static int acl_delete(struct ldb_module *module, struct ldb_request *req)
{
	int ret;
	struct ldb_dn *parent;
	struct ldb_context *ldb;
	struct ldb_dn *nc_root;
	struct ldb_control *as_system;
	const struct dsdb_schema *schema;
	const struct dsdb_class *objectclass;
	struct security_descriptor *sd = NULL;
	struct dom_sid *sid = NULL;
	struct ldb_result *acl_res;
	static const char *acl_attrs[] = {
		"nTSecurityDescriptor",
		"objectClass",
		"objectSid",
		NULL
	};

	if (ldb_dn_is_special(req->op.del.dn)) {
		return ldb_next_request(module, req);
	}

	as_system = ldb_request_get_control(req, LDB_CONTROL_AS_SYSTEM_OID);
	if (as_system != NULL) {
		as_system->critical = 0;
	}

	if (dsdb_module_am_system(module) || as_system) {
		return ldb_next_request(module, req);
	}

	DEBUG(10, ("ldb:acl_delete: %s\n", ldb_dn_get_linearized(req->op.del.dn)));

	ldb = ldb_module_get_ctx(module);

	parent = ldb_dn_get_parent(req, req->op.del.dn);
	if (parent == NULL) {
		return ldb_oom(ldb);
	}

	/* Make sure we aren't deleting a NC */

	ret = dsdb_find_nc_root(ldb, req, req->op.del.dn, &nc_root);
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	if (ldb_dn_compare(nc_root, req->op.del.dn) == 0) {
		talloc_free(nc_root);
		DEBUG(10,("acl:deleting a NC\n"));
		/* Windows returns "ERR_UNWILLING_TO_PERFORM */
		return ldb_module_done(req, NULL, NULL,
				       LDB_ERR_UNWILLING_TO_PERFORM);
	}
	talloc_free(nc_root);

	ret = dsdb_module_search_dn(module, req, &acl_res,
				    req->op.del.dn, acl_attrs,
				    DSDB_FLAG_NEXT_MODULE |
				    DSDB_FLAG_AS_SYSTEM |
				    DSDB_SEARCH_SHOW_RECYCLED, req);
	/* we sould be able to find the parent */
	if (ret != LDB_SUCCESS) {
		DEBUG(10,("acl: failed to find object %s\n",
			  ldb_dn_get_linearized(req->op.rename.olddn)));
		return ret;
	}

	ret = dsdb_get_sd_from_ldb_message(ldb, req, acl_res->msgs[0], &sd);
	if (ret != LDB_SUCCESS) {
		return ldb_operr(ldb);
	}
	if (!sd) {
		return ldb_operr(ldb);
	}

	schema = dsdb_get_schema(ldb, req);
	if (!schema) {
		return ldb_operr(ldb);
	}

	sid = samdb_result_dom_sid(req, acl_res->msgs[0], "objectSid");

	objectclass = dsdb_get_structural_oc_from_msg(schema, acl_res->msgs[0]);
	if (!objectclass) {
		return ldb_error(ldb, LDB_ERR_OPERATIONS_ERROR,
				 "acl_modify: Error retrieving object class for GUID.");
	}

	if (ldb_request_get_control(req, LDB_CONTROL_TREE_DELETE_OID)) {
		ret = acl_check_access_on_objectclass(module, req, sd, sid,
						      SEC_ADS_DELETE_TREE,
						      objectclass);
		if (ret != LDB_SUCCESS) {
			return ret;
		}

		return ldb_next_request(module, req);
	}

	/* First check if we have delete object right */
	ret = acl_check_access_on_objectclass(module, req, sd, sid,
					      SEC_STD_DELETE,
					      objectclass);
	if (ret == LDB_SUCCESS) {
		return ldb_next_request(module, req);
	}

	/* Nope, we don't have delete object. Lets check if we have delete
	 * child on the parent */
	ret = dsdb_module_check_access_on_dn(module, req, parent,
					     SEC_ADS_DELETE_CHILD,
					     &objectclass->schemaIDGUID,
					     req);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	return ldb_next_request(module, req);
}
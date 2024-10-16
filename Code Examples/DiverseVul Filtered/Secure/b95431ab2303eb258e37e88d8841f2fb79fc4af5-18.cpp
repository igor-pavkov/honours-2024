static int acl_rename(struct ldb_module *module, struct ldb_request *req)
{
	int ret;
	struct ldb_dn *oldparent;
	struct ldb_dn *newparent;
	const struct dsdb_schema *schema;
	const struct dsdb_class *objectclass;
	const struct dsdb_attribute *attr = NULL;
	struct ldb_context *ldb;
	struct security_descriptor *sd = NULL;
	struct dom_sid *sid = NULL;
	struct ldb_result *acl_res;
	struct ldb_dn *nc_root;
	struct ldb_control *as_system;
	struct ldb_control *is_undelete;
	TALLOC_CTX *tmp_ctx;
	const char *rdn_name;
	static const char *acl_attrs[] = {
		"nTSecurityDescriptor",
		"objectClass",
		"objectSid",
		NULL
	};

	if (ldb_dn_is_special(req->op.rename.olddn)) {
		return ldb_next_request(module, req);
	}

	as_system = ldb_request_get_control(req, LDB_CONTROL_AS_SYSTEM_OID);
	if (as_system != NULL) {
		as_system->critical = 0;
	}

	DEBUG(10, ("ldb:acl_rename: %s\n", ldb_dn_get_linearized(req->op.rename.olddn)));
	if (dsdb_module_am_system(module) || as_system) {
		return ldb_next_request(module, req);
	}

	ldb = ldb_module_get_ctx(module);

	tmp_ctx = talloc_new(req);
	if (tmp_ctx == NULL) {
		return ldb_oom(ldb);
	}

	oldparent = ldb_dn_get_parent(tmp_ctx, req->op.rename.olddn);
	if (oldparent == NULL) {
		return ldb_oom(ldb);
	}
	newparent = ldb_dn_get_parent(tmp_ctx, req->op.rename.newdn);
	if (newparent == NULL) {
		return ldb_oom(ldb);
	}

	/* Make sure we aren't renaming/moving a NC */

	ret = dsdb_find_nc_root(ldb, req, req->op.rename.olddn, &nc_root);
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	if (ldb_dn_compare(nc_root, req->op.rename.olddn) == 0) {
		talloc_free(nc_root);
		DEBUG(10,("acl:renaming/moving a NC\n"));
		/* Windows returns "ERR_UNWILLING_TO_PERFORM */
		return ldb_module_done(req, NULL, NULL,
				       LDB_ERR_UNWILLING_TO_PERFORM);
	}

	/* special check for undelete operation */
	is_undelete = ldb_request_get_control(req, DSDB_CONTROL_RESTORE_TOMBSTONE_OID);
	if (is_undelete != NULL) {
		is_undelete->critical = 0;
		ret = acl_check_reanimate_tombstone(tmp_ctx, module, req, nc_root);
		if (ret != LDB_SUCCESS) {
			talloc_free(tmp_ctx);
			return ret;
		}
	}
	talloc_free(nc_root);

	/* Look for the parent */

	ret = dsdb_module_search_dn(module, tmp_ctx, &acl_res,
				    req->op.rename.olddn, acl_attrs,
				    DSDB_FLAG_NEXT_MODULE |
				    DSDB_FLAG_AS_SYSTEM |
				    DSDB_SEARCH_SHOW_RECYCLED, req);
	/* we sould be able to find the parent */
	if (ret != LDB_SUCCESS) {
		DEBUG(10,("acl: failed to find object %s\n",
			  ldb_dn_get_linearized(req->op.rename.olddn)));
		talloc_free(tmp_ctx);
		return ret;
	}

	ret = dsdb_get_sd_from_ldb_message(ldb, req, acl_res->msgs[0], &sd);
	if (ret != LDB_SUCCESS) {
		talloc_free(tmp_ctx);
		return ldb_operr(ldb);
	}
	if (!sd) {
		talloc_free(tmp_ctx);
		return ldb_operr(ldb);
	}

	schema = dsdb_get_schema(ldb, acl_res);
	if (!schema) {
		talloc_free(tmp_ctx);
		return ldb_operr(ldb);
	}

	sid = samdb_result_dom_sid(req, acl_res->msgs[0], "objectSid");

	objectclass = dsdb_get_structural_oc_from_msg(schema, acl_res->msgs[0]);
	if (!objectclass) {
		talloc_free(tmp_ctx);
		return ldb_error(ldb, LDB_ERR_OPERATIONS_ERROR,
				 "acl_modify: Error retrieving object class for GUID.");
	}

	attr = dsdb_attribute_by_lDAPDisplayName(schema, "name");
	if (attr == NULL) {
		talloc_free(tmp_ctx);
		return ldb_operr(ldb);
	}

	ret = acl_check_access_on_attribute(module, tmp_ctx, sd, sid,
					    SEC_ADS_WRITE_PROP,
					    attr, objectclass);
	if (ret != LDB_SUCCESS) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       "Object %s has no wp on %s\n",
				       ldb_dn_get_linearized(req->op.rename.olddn),
				       attr->lDAPDisplayName);
		dsdb_acl_debug(sd,
			  acl_user_token(module),
			  req->op.rename.olddn,
			  true,
			  10);
		talloc_free(tmp_ctx);
		return LDB_ERR_INSUFFICIENT_ACCESS_RIGHTS;
	}

	rdn_name = ldb_dn_get_rdn_name(req->op.rename.olddn);
	if (rdn_name == NULL) {
		talloc_free(tmp_ctx);
		return ldb_operr(ldb);
	}

	attr = dsdb_attribute_by_lDAPDisplayName(schema, rdn_name);
	if (attr == NULL) {
		talloc_free(tmp_ctx);
		return ldb_operr(ldb);
	}

	ret = acl_check_access_on_attribute(module, tmp_ctx, sd, sid,
					    SEC_ADS_WRITE_PROP,
					    attr, objectclass);
	if (ret != LDB_SUCCESS) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       "Object %s has no wp on %s\n",
				       ldb_dn_get_linearized(req->op.rename.olddn),
				       attr->lDAPDisplayName);
		dsdb_acl_debug(sd,
			  acl_user_token(module),
			  req->op.rename.olddn,
			  true,
			  10);
		talloc_free(tmp_ctx);
		return LDB_ERR_INSUFFICIENT_ACCESS_RIGHTS;
	}

	if (ldb_dn_compare(oldparent, newparent) == 0) {
		/* regular rename, not move, nothing more to do */
		talloc_free(tmp_ctx);
		return ldb_next_request(module, req);
	}

	/* new parent should have create child */
	ret = dsdb_module_check_access_on_dn(module, req, newparent,
					     SEC_ADS_CREATE_CHILD,
					     &objectclass->schemaIDGUID, req);
	if (ret != LDB_SUCCESS) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       "acl:access_denied renaming %s",
				       ldb_dn_get_linearized(req->op.rename.olddn));
		talloc_free(tmp_ctx);
		return ret;
	}

	/* do we have delete object on the object? */
	/* this access is not necessary for undelete ops */
	if (is_undelete == NULL) {
		ret = acl_check_access_on_objectclass(module, tmp_ctx, sd, sid,
						      SEC_STD_DELETE,
						      objectclass);
		if (ret == LDB_SUCCESS) {
			talloc_free(tmp_ctx);
			return ldb_next_request(module, req);
		}
		/* what about delete child on the current parent */
		ret = dsdb_module_check_access_on_dn(module, req, oldparent,
						     SEC_ADS_DELETE_CHILD,
						     &objectclass->schemaIDGUID,
						     req);
		if (ret != LDB_SUCCESS) {
			ldb_asprintf_errstring(ldb_module_get_ctx(module),
					       "acl:access_denied renaming %s", ldb_dn_get_linearized(req->op.rename.olddn));
			talloc_free(tmp_ctx);
			return ldb_module_done(req, NULL, NULL, ret);
		}
	}
	talloc_free(tmp_ctx);

	return ldb_next_request(module, req);
}
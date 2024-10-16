static int acl_search(struct ldb_module *module, struct ldb_request *req)
{
	struct ldb_context *ldb;
	struct acl_context *ac;
	struct ldb_parse_tree *down_tree;
	struct ldb_request *down_req;
	struct acl_private *data;
	int ret;
	unsigned int i;

	if (ldb_dn_is_special(req->op.search.base)) {
		return ldb_next_request(module, req);
	}

	ldb = ldb_module_get_ctx(module);

	ac = talloc_zero(req, struct acl_context);
	if (ac == NULL) {
		return ldb_oom(ldb);
	}
	data = talloc_get_type(ldb_module_get_private(module), struct acl_private);

	ac->module = module;
	ac->req = req;
	ac->am_system = dsdb_module_am_system(module);
	ac->am_administrator = dsdb_module_am_administrator(module);
	ac->constructed_attrs = false;
	ac->modify_search = true;
	ac->allowedAttributes = ldb_attr_in_list(req->op.search.attrs, "allowedAttributes");
	ac->allowedAttributesEffective = ldb_attr_in_list(req->op.search.attrs, "allowedAttributesEffective");
	ac->allowedChildClasses = ldb_attr_in_list(req->op.search.attrs, "allowedChildClasses");
	ac->allowedChildClassesEffective = ldb_attr_in_list(req->op.search.attrs, "allowedChildClassesEffective");
	ac->sDRightsEffective = ldb_attr_in_list(req->op.search.attrs, "sDRightsEffective");
	ac->userPassword = true;
	ac->schema = dsdb_get_schema(ldb, ac);

	ac->constructed_attrs |= ac->allowedAttributes;
	ac->constructed_attrs |= ac->allowedChildClasses;
	ac->constructed_attrs |= ac->allowedChildClassesEffective;
	ac->constructed_attrs |= ac->allowedAttributesEffective;
	ac->constructed_attrs |= ac->sDRightsEffective;

	if (data == NULL) {
		ac->modify_search = false;
	}
	if (ac->am_system) {
		ac->modify_search = false;
	}

	if (!ac->constructed_attrs && !ac->modify_search) {
		talloc_free(ac);
		return ldb_next_request(module, req);
	}

	data = talloc_get_type(ldb_module_get_private(ac->module), struct acl_private);
	if (data == NULL) {
		return ldb_error(ldb, LDB_ERR_OPERATIONS_ERROR,
				 "acl_private data is missing");
	}
	ac->userPassword = data->userPassword_support;

	ret = acl_search_update_confidential_attrs(ac, data);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	down_tree = ldb_parse_tree_copy_shallow(ac, req->op.search.tree);
	if (down_tree == NULL) {
		return ldb_oom(ldb);
	}

	if (!ac->am_system && data->password_attrs) {
		for (i = 0; data->password_attrs[i]; i++) {
			if ((!ac->userPassword) &&
			    (ldb_attr_cmp(data->password_attrs[i],
					  "userPassword") == 0))
			{
				continue;
			}

			ldb_parse_tree_attr_replace(down_tree,
						    data->password_attrs[i],
						    "kludgeACLredactedattribute");
		}
	}

	if (!ac->am_system && !ac->am_administrator && data->confidential_attrs) {
		for (i = 0; data->confidential_attrs[i]; i++) {
			ldb_parse_tree_attr_replace(down_tree,
						    data->confidential_attrs[i],
						    "kludgeACLredactedattribute");
		}
	}

	ret = ldb_build_search_req_ex(&down_req,
				      ldb, ac,
				      req->op.search.base,
				      req->op.search.scope,
				      down_tree,
				      req->op.search.attrs,
				      req->controls,
				      ac, acl_search_callback,
				      req);
	LDB_REQ_SET_LOCATION(down_req);
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	/* perform the search */
	return ldb_next_request(module, down_req);
}
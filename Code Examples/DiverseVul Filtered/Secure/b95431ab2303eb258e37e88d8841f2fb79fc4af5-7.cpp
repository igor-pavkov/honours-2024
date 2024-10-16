static int acl_search_callback(struct ldb_request *req, struct ldb_reply *ares)
{
	struct acl_context *ac;
	struct acl_private *data;
	struct ldb_result *acl_res;
	static const char *acl_attrs[] = {
		"objectClass",
		"nTSecurityDescriptor",
		"objectSid",
		NULL
	};
	int ret;
	unsigned int i;

	ac = talloc_get_type(req->context, struct acl_context);
	data = talloc_get_type(ldb_module_get_private(ac->module), struct acl_private);
	if (!ares) {
		return ldb_module_done(ac->req, NULL, NULL,
				       LDB_ERR_OPERATIONS_ERROR);
	}
	if (ares->error != LDB_SUCCESS) {
		return ldb_module_done(ac->req, ares->controls,
				       ares->response, ares->error);
	}

	switch (ares->type) {
	case LDB_REPLY_ENTRY:
		if (ac->constructed_attrs) {
			ret = dsdb_module_search_dn(ac->module, ac, &acl_res, ares->message->dn, 
						    acl_attrs,
						    DSDB_FLAG_NEXT_MODULE |
						    DSDB_FLAG_AS_SYSTEM |
						    DSDB_SEARCH_SHOW_RECYCLED,
						    req);
			if (ret != LDB_SUCCESS) {
				return ldb_module_done(ac->req, NULL, NULL, ret);
			}
		}

		if (ac->allowedAttributes || ac->allowedAttributesEffective) {
			ret = acl_allowedAttributes(ac->module, ac->schema,
						    acl_res->msgs[0],
						    ares->message, ac);
			if (ret != LDB_SUCCESS) {
				return ldb_module_done(ac->req, NULL, NULL, ret);
			}
		}

		if (ac->allowedChildClasses) {
			ret = acl_childClasses(ac->module, ac->schema,
					       acl_res->msgs[0],
					       ares->message,
					       "allowedChildClasses");
			if (ret != LDB_SUCCESS) {
				return ldb_module_done(ac->req, NULL, NULL, ret);
			}
		}

		if (ac->allowedChildClassesEffective) {
			ret = acl_childClassesEffective(ac->module, ac->schema,
							acl_res->msgs[0],
							ares->message, ac);
			if (ret != LDB_SUCCESS) {
				return ldb_module_done(ac->req, NULL, NULL, ret);
			}
		}

		if (ac->sDRightsEffective) {
			ret = acl_sDRightsEffective(ac->module,
						    acl_res->msgs[0],
						    ares->message, ac);
			if (ret != LDB_SUCCESS) {
				return ldb_module_done(ac->req, NULL, NULL, ret);
			}
		}

		if (data == NULL) {
			return ldb_module_send_entry(ac->req, ares->message,
						     ares->controls);
		}

		if (ac->am_system) {
			return ldb_module_send_entry(ac->req, ares->message,
						     ares->controls);
		}

		if (data->password_attrs != NULL) {
			for (i = 0; data->password_attrs[i]; i++) {
				if ((!ac->userPassword) &&
				    (ldb_attr_cmp(data->password_attrs[i],
						  "userPassword") == 0))
				{
						continue;
				}

				ldb_msg_remove_attr(ares->message, data->password_attrs[i]);
			}
		}

		if (ac->am_administrator) {
			return ldb_module_send_entry(ac->req, ares->message,
						     ares->controls);
		}

		ret = acl_search_update_confidential_attrs(ac, data);
		if (ret != LDB_SUCCESS) {
			return ret;
		}

		if (data->confidential_attrs != NULL) {
			for (i = 0; data->confidential_attrs[i]; i++) {
				ldb_msg_remove_attr(ares->message,
						    data->confidential_attrs[i]);
			}
		}

		return ldb_module_send_entry(ac->req, ares->message, ares->controls);

	case LDB_REPLY_REFERRAL:
		return ldb_module_send_referral(ac->req, ares->referral);

	case LDB_REPLY_DONE:
		return ldb_module_done(ac->req, ares->controls,
				       ares->response, LDB_SUCCESS);

	}
	return LDB_SUCCESS;
}
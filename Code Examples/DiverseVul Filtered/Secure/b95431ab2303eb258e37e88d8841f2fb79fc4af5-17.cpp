static int acl_module_init(struct ldb_module *module)
{
	struct ldb_context *ldb;
	struct acl_private *data;
	int ret;
	unsigned int i, n, j;
	TALLOC_CTX *mem_ctx;
	static const char * const attrs[] = { "passwordAttribute", NULL };
	static const char * const secret_attrs[] = {
		DSDB_SECRET_ATTRIBUTES
	};
	struct ldb_result *res;
	struct ldb_message *msg;
	struct ldb_message_element *password_attributes;

	ldb = ldb_module_get_ctx(module);

	ret = ldb_mod_register_control(module, LDB_CONTROL_SD_FLAGS_OID);
	if (ret != LDB_SUCCESS) {
		ldb_debug(ldb, LDB_DEBUG_ERROR,
			  "acl_module_init: Unable to register control with rootdse!\n");
		return ldb_operr(ldb);
	}

	data = talloc_zero(module, struct acl_private);
	if (data == NULL) {
		return ldb_oom(ldb);
	}

	data->acl_search = lpcfg_parm_bool(ldb_get_opaque(ldb, "loadparm"),
					NULL, "acl", "search", true);
	ldb_module_set_private(module, data);

	mem_ctx = talloc_new(module);
	if (!mem_ctx) {
		return ldb_oom(ldb);
	}

	ret = dsdb_module_search_dn(module, mem_ctx, &res,
				    ldb_dn_new(mem_ctx, ldb, "@KLUDGEACL"),
				    attrs,
				    DSDB_FLAG_NEXT_MODULE |
				    DSDB_FLAG_AS_SYSTEM,
				    NULL);
	if (ret != LDB_SUCCESS) {
		goto done;
	}
	if (res->count == 0) {
		goto done;
	}

	if (res->count > 1) {
		talloc_free(mem_ctx);
		return LDB_ERR_CONSTRAINT_VIOLATION;
	}

	msg = res->msgs[0];

	password_attributes = ldb_msg_find_element(msg, "passwordAttribute");
	if (!password_attributes) {
		goto done;
	}
	data->password_attrs = talloc_array(data, const char *,
			password_attributes->num_values +
			ARRAY_SIZE(secret_attrs) + 1);
	if (!data->password_attrs) {
		talloc_free(mem_ctx);
		return ldb_oom(ldb);
	}

	n = 0;
	for (i=0; i < password_attributes->num_values; i++) {
		data->password_attrs[n] = (const char *)password_attributes->values[i].data;
		talloc_steal(data->password_attrs, password_attributes->values[i].data);
		n++;
	}

	for (i=0; i < ARRAY_SIZE(secret_attrs); i++) {
		bool found = false;

		for (j=0; j < n; j++) {
			if (strcasecmp(data->password_attrs[j], secret_attrs[i]) == 0) {
				found = true;
				break;
			}
		}

		if (found) {
			continue;
		}

		data->password_attrs[n] = talloc_strdup(data->password_attrs,
							secret_attrs[i]);
		if (data->password_attrs[n] == NULL) {
			talloc_free(mem_ctx);
			return ldb_oom(ldb);
		}
		n++;
	}
	data->password_attrs[n] = NULL;

done:
	talloc_free(mem_ctx);
	ret = ldb_next_init(module);

	if (ret != LDB_SUCCESS) {
		return ret;
	}

	/*
	 * Check this after the modules have be initialised so we
	 * can actually read the backend DB.
	 */
	data->userPassword_support
		= dsdb_user_password_support(module,
					     module,
					     NULL);
	return ret;
}
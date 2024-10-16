static int acl_allowedAttributes(struct ldb_module *module,
				 const struct dsdb_schema *schema,
				 struct ldb_message *sd_msg,
				 struct ldb_message *msg,
				 struct acl_context *ac)
{
	struct ldb_message_element *oc_el;
	struct ldb_context *ldb = ldb_module_get_ctx(module);
	TALLOC_CTX *mem_ctx;
	const char **attr_list;
	int i, ret;
	const struct dsdb_class *objectclass;

	/* If we don't have a schema yet, we can't do anything... */
	if (schema == NULL) {
		ldb_asprintf_errstring(ldb, "cannot add allowedAttributes to %s because no schema is loaded", ldb_dn_get_linearized(msg->dn));
		return LDB_ERR_OPERATIONS_ERROR;
	}

	/* Must remove any existing attribute */
	if (ac->allowedAttributes) {
		ldb_msg_remove_attr(msg, "allowedAttributes");
	}

	mem_ctx = talloc_new(msg);
	if (!mem_ctx) {
		return ldb_oom(ldb);
	}

	oc_el = ldb_msg_find_element(sd_msg, "objectClass");
	attr_list = dsdb_full_attribute_list(mem_ctx, schema, oc_el, DSDB_SCHEMA_ALL);
	if (!attr_list) {
		ldb_asprintf_errstring(ldb, "acl: Failed to get list of attributes");
		talloc_free(mem_ctx);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	/*
	 * Get the top-most structural object class for the ACL check
	 */
	objectclass = dsdb_get_last_structural_class(ac->schema,
						     oc_el);
	if (objectclass == NULL) {
		ldb_asprintf_errstring(ldb, "acl_read: Failed to find a structural class for %s",
				       ldb_dn_get_linearized(sd_msg->dn));
		talloc_free(mem_ctx);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	if (ac->allowedAttributes) {
		for (i=0; attr_list && attr_list[i]; i++) {
			ldb_msg_add_string(msg, "allowedAttributes", attr_list[i]);
		}
	}
	if (ac->allowedAttributesEffective) {
		struct security_descriptor *sd;
		struct dom_sid *sid = NULL;
		struct ldb_control *as_system = ldb_request_get_control(ac->req,
									LDB_CONTROL_AS_SYSTEM_OID);

		if (as_system != NULL) {
			as_system->critical = 0;
		}

		ldb_msg_remove_attr(msg, "allowedAttributesEffective");
		if (ac->am_system || as_system) {
			for (i=0; attr_list && attr_list[i]; i++) {
				ldb_msg_add_string(msg, "allowedAttributesEffective", attr_list[i]);
			}
			return LDB_SUCCESS;
		}

		ret = dsdb_get_sd_from_ldb_message(ldb_module_get_ctx(module), mem_ctx, sd_msg, &sd);

		if (ret != LDB_SUCCESS) {
			return ret;
		}

		sid = samdb_result_dom_sid(mem_ctx, sd_msg, "objectSid");
		for (i=0; attr_list && attr_list[i]; i++) {
			const struct dsdb_attribute *attr = dsdb_attribute_by_lDAPDisplayName(schema,
											attr_list[i]);
			if (!attr) {
				return ldb_operr(ldb);
			}
			/* remove constructed attributes */
			if (attr->systemFlags & DS_FLAG_ATTR_IS_CONSTRUCTED
			    || attr->systemOnly
			    || (attr->linkID != 0 && attr->linkID % 2 != 0 )) {
				continue;
			}
			ret = acl_check_access_on_attribute(module,
							    msg,
							    sd,
							    sid,
							    SEC_ADS_WRITE_PROP,
							    attr,
							    objectclass);
			if (ret == LDB_SUCCESS) {
				ldb_msg_add_string(msg, "allowedAttributesEffective", attr_list[i]);
			}
		}
	}
	return LDB_SUCCESS;
}
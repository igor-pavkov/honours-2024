static int acl_childClassesEffective(struct ldb_module *module,
				     const struct dsdb_schema *schema,
				     struct ldb_message *sd_msg,
				     struct ldb_message *msg,
				     struct acl_context *ac)
{
	struct ldb_message_element *oc_el;
	struct ldb_message_element *allowedClasses = NULL;
	const struct dsdb_class *sclass;
	struct security_descriptor *sd;
	struct ldb_control *as_system = ldb_request_get_control(ac->req,
								LDB_CONTROL_AS_SYSTEM_OID);
	struct dom_sid *sid = NULL;
	unsigned int i, j;
	int ret;

	if (as_system != NULL) {
		as_system->critical = 0;
	}

	if (ac->am_system || as_system) {
		return acl_childClasses(module, schema, sd_msg, msg, "allowedChildClassesEffective");
	}

	/* If we don't have a schema yet, we can't do anything... */
	if (schema == NULL) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module), "cannot add allowedChildClassesEffective to %s because no schema is loaded", ldb_dn_get_linearized(msg->dn));
		return LDB_ERR_OPERATIONS_ERROR;
	}

	/* Must remove any existing attribute, or else confusion reins */
	ldb_msg_remove_attr(msg, "allowedChildClassesEffective");

	oc_el = ldb_msg_find_element(sd_msg, "objectClass");
	ret = dsdb_get_sd_from_ldb_message(ldb_module_get_ctx(module), msg, sd_msg, &sd);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	sid = samdb_result_dom_sid(msg, sd_msg, "objectSid");
	for (i=0; oc_el && i < oc_el->num_values; i++) {
		sclass = dsdb_class_by_lDAPDisplayName_ldb_val(schema, &oc_el->values[i]);
		if (!sclass) {
			/* We don't know this class?  what is going on? */
			continue;
		}

		for (j=0; sclass->possibleInferiors && sclass->possibleInferiors[j]; j++) {
			const struct dsdb_class *sc;

			sc = dsdb_class_by_lDAPDisplayName(schema,
							   sclass->possibleInferiors[j]);
			if (!sc) {
				/* We don't know this class?  what is going on? */
				continue;
			}

			ret = acl_check_access_on_objectclass(module, ac,
							      sd, sid,
							      SEC_ADS_CREATE_CHILD,
							      sc);
			if (ret == LDB_SUCCESS) {
				ldb_msg_add_string(msg, "allowedChildClassesEffective",
						   sclass->possibleInferiors[j]);
			}
		}
	}
	allowedClasses = ldb_msg_find_element(msg, "allowedChildClassesEffective");
	if (!allowedClasses) {
		return LDB_SUCCESS;
	}

	if (allowedClasses->num_values > 1) {
		TYPESAFE_QSORT(allowedClasses->values, allowedClasses->num_values, data_blob_cmp);
		for (i=1 ; i < allowedClasses->num_values; i++) {
			struct ldb_val *val1 = &allowedClasses->values[i-1];
			struct ldb_val *val2 = &allowedClasses->values[i];
			if (data_blob_cmp(val1, val2) == 0) {
				memmove(val1, val2, (allowedClasses->num_values - i) * sizeof( struct ldb_val));
				allowedClasses->num_values--;
				i--;
			}
		}
	}
	return LDB_SUCCESS;
}
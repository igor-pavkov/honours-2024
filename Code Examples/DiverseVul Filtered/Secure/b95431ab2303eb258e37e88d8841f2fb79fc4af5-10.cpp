static int acl_childClasses(struct ldb_module *module,
			    const struct dsdb_schema *schema,
			    struct ldb_message *sd_msg,
			    struct ldb_message *msg,
			    const char *attrName)
{
	struct ldb_message_element *oc_el;
	struct ldb_message_element *allowedClasses;
	const struct dsdb_class *sclass;
	unsigned int i, j;
	int ret;

	/* If we don't have a schema yet, we can't do anything... */
	if (schema == NULL) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module), "cannot add childClassesEffective to %s because no schema is loaded", ldb_dn_get_linearized(msg->dn));
		return LDB_ERR_OPERATIONS_ERROR;
	}

	/* Must remove any existing attribute, or else confusion reins */
	ldb_msg_remove_attr(msg, attrName);
	ret = ldb_msg_add_empty(msg, attrName, 0, &allowedClasses);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	oc_el = ldb_msg_find_element(sd_msg, "objectClass");

	for (i=0; oc_el && i < oc_el->num_values; i++) {
		sclass = dsdb_class_by_lDAPDisplayName_ldb_val(schema, &oc_el->values[i]);
		if (!sclass) {
			/* We don't know this class?  what is going on? */
			continue;
		}

		for (j=0; sclass->possibleInferiors && sclass->possibleInferiors[j]; j++) {
			ldb_msg_add_string(msg, attrName, sclass->possibleInferiors[j]);
		}
	}
	if (allowedClasses->num_values > 1) {
		TYPESAFE_QSORT(allowedClasses->values, allowedClasses->num_values, data_blob_cmp);
		for (i=1 ; i < allowedClasses->num_values; i++) {
			struct ldb_val *val1 = &allowedClasses->values[i-1];
			struct ldb_val *val2 = &allowedClasses->values[i];
			if (data_blob_cmp(val1, val2) == 0) {
				memmove(val1, val2, (allowedClasses->num_values - i) * sizeof(struct ldb_val));
				allowedClasses->num_values--;
				i--;
			}
		}
	}

	return LDB_SUCCESS;
}
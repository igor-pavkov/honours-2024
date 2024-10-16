static int acl_sDRightsEffective(struct ldb_module *module,
				 struct ldb_message *sd_msg,
				 struct ldb_message *msg,
				 struct acl_context *ac)
{
	struct ldb_context *ldb = ldb_module_get_ctx(module);
	struct ldb_message_element *rightsEffective;
	int ret;
	struct security_descriptor *sd;
	struct ldb_control *as_system = ldb_request_get_control(ac->req,
								LDB_CONTROL_AS_SYSTEM_OID);
	struct dom_sid *sid = NULL;
	uint32_t flags = 0;

	if (as_system != NULL) {
		as_system->critical = 0;
	}

	/* Must remove any existing attribute, or else confusion reins */
	ldb_msg_remove_attr(msg, "sDRightsEffective");
	ret = ldb_msg_add_empty(msg, "sDRightsEffective", 0, &rightsEffective);
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	if (ac->am_system || as_system) {
		flags = SECINFO_OWNER | SECINFO_GROUP |  SECINFO_SACL |  SECINFO_DACL;
	} else {
		const struct dsdb_class *objectclass;
		const struct dsdb_attribute *attr;

		objectclass = dsdb_get_structural_oc_from_msg(ac->schema, sd_msg);
		if (objectclass == NULL) {
			return ldb_operr(ldb);
		}

		attr = dsdb_attribute_by_lDAPDisplayName(ac->schema,
							 "nTSecurityDescriptor");
		if (attr == NULL) {
			return ldb_operr(ldb);
		}

		/* Get the security descriptor from the message */
		ret = dsdb_get_sd_from_ldb_message(ldb, msg, sd_msg, &sd);
		if (ret != LDB_SUCCESS) {
			return ret;
		}
		sid = samdb_result_dom_sid(msg, sd_msg, "objectSid");
		ret = acl_check_access_on_attribute(module,
						    msg,
						    sd,
						    sid,
						    SEC_STD_WRITE_OWNER,
						    attr,
						    objectclass);
		if (ret == LDB_SUCCESS) {
			flags |= SECINFO_OWNER | SECINFO_GROUP;
		}
		ret = acl_check_access_on_attribute(module,
						    msg,
						    sd,
						    sid,
						    SEC_STD_WRITE_DAC,
						    attr,
						    objectclass);
		if (ret == LDB_SUCCESS) {
			flags |= SECINFO_DACL;
		}
		ret = acl_check_access_on_attribute(module,
						    msg,
						    sd,
						    sid,
						    SEC_FLAG_SYSTEM_SECURITY,
						    attr,
						    objectclass);
		if (ret == LDB_SUCCESS) {
			flags |= SECINFO_SACL;
		}
	}
	return samdb_msg_add_uint(ldb_module_get_ctx(module), msg, msg,
				  "sDRightsEffective", flags);
}
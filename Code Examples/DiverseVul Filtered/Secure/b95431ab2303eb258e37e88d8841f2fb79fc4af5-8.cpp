static int acl_check_self_membership(TALLOC_CTX *mem_ctx,
				     struct ldb_module *module,
				     struct ldb_request *req,
				     struct security_descriptor *sd,
				     struct dom_sid *sid,
				     const struct dsdb_attribute *attr,
				     const struct dsdb_class *objectclass)
{
	int ret;
	unsigned int i;
	struct ldb_context *ldb = ldb_module_get_ctx(module);
	struct ldb_dn *user_dn;
	struct ldb_message_element *member_el;
	const struct ldb_message *msg = NULL;

	if (req->operation == LDB_MODIFY) {
		msg = req->op.mod.message;
	} else if (req->operation == LDB_ADD) {
		msg = req->op.add.message;
	} else {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	/* if we have wp, we can do whatever we like */
	if (acl_check_access_on_attribute(module,
					  mem_ctx,
					  sd,
					  sid,
					  SEC_ADS_WRITE_PROP,
					  attr, objectclass) == LDB_SUCCESS) {
		return LDB_SUCCESS;
	}
	/* if we are adding/deleting ourselves, check for self membership */
	ret = dsdb_find_dn_by_sid(ldb, mem_ctx,
				  &acl_user_token(module)->sids[PRIMARY_USER_SID_INDEX],
				  &user_dn);
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	member_el = ldb_msg_find_element(msg, "member");
	if (!member_el) {
		return ldb_operr(ldb);
	}
	/* user can only remove oneself */
	if (member_el->num_values == 0) {
		return LDB_ERR_INSUFFICIENT_ACCESS_RIGHTS;
	}
	for (i = 0; i < member_el->num_values; i++) {
		if (strcasecmp((const char *)member_el->values[i].data,
			       ldb_dn_get_extended_linearized(mem_ctx, user_dn, 1)) != 0) {
			return LDB_ERR_INSUFFICIENT_ACCESS_RIGHTS;
		}
	}
	ret = acl_check_extended_right(mem_ctx,
				       module,
				       req,
				       objectclass,
				       sd,
				       acl_user_token(module),
				       GUID_DRS_SELF_MEMBERSHIP,
				       SEC_ADS_SELF_WRITE,
				       sid);
	if (ret == LDB_ERR_INSUFFICIENT_ACCESS_RIGHTS) {
		dsdb_acl_debug(sd, acl_user_token(module),
			       msg->dn,
			       true,
			       10);
	}
	return ret;
}
static int acl_extended(struct ldb_module *module, struct ldb_request *req)
{
	struct ldb_context *ldb = ldb_module_get_ctx(module);
	struct ldb_control *as_system = ldb_request_get_control(req, LDB_CONTROL_AS_SYSTEM_OID);

	/* allow everybody to read the sequence number */
	if (strcmp(req->op.extended.oid,
		   LDB_EXTENDED_SEQUENCE_NUMBER) == 0) {
		return ldb_next_request(module, req);
	}

	if (dsdb_module_am_system(module) ||
	    dsdb_module_am_administrator(module) || as_system) {
		return ldb_next_request(module, req);
	} else {
		ldb_asprintf_errstring(ldb,
				       "acl_extended: "
				       "attempted database modify not permitted. "
				       "User %s is not SYSTEM or an administrator",
				       acl_user_name(req, module));
		return LDB_ERR_INSUFFICIENT_ACCESS_RIGHTS;
	}
}
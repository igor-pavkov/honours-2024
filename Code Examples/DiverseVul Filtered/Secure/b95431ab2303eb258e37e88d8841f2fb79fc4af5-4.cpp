static int acl_callback(struct ldb_request *req, struct ldb_reply *ares)
{
	struct acl_callback_context *ac = NULL;

	ac = talloc_get_type(req->context, struct acl_callback_context);

	if (!ares) {
		return ldb_module_done(
			ac->request,
			NULL,
			NULL,
			LDB_ERR_OPERATIONS_ERROR);
	}

	/* pass on to the callback */
	switch (ares->type) {
	case LDB_REPLY_ENTRY:
		return ldb_module_send_entry(
			ac->request,
			ares->message,
			ares->controls);

	case LDB_REPLY_REFERRAL:
		return ldb_module_send_referral(
			ac->request,
			ares->referral);

	case LDB_REPLY_DONE:
		/*
		 * Copy the ACL control from the request to the response
		 */
		copy_password_acl_validation_control(req, ares);
		return ldb_module_done(
			ac->request,
			ares->controls,
			ares->response,
			ares->error);

	default:
		/* Can't happen */
		return LDB_ERR_OPERATIONS_ERROR;
	}
}
static void copy_password_acl_validation_control(
	struct ldb_request *req,
	struct ldb_reply *ares)
{
	struct ldb_control *pav_ctrl = NULL;
	struct dsdb_control_password_acl_validation *pav = NULL;

	pav_ctrl = ldb_request_get_control(
		discard_const(req),
		DSDB_CONTROL_PASSWORD_ACL_VALIDATION_OID);
	if (pav_ctrl == NULL) {
		return;
	}

	pav = talloc_get_type_abort(
		pav_ctrl->data,
		struct dsdb_control_password_acl_validation);
	if (pav == NULL) {
		return;
	}
	ldb_reply_add_control(
		ares,
		DSDB_CONTROL_PASSWORD_ACL_VALIDATION_OID,
		false,
		pav);
}
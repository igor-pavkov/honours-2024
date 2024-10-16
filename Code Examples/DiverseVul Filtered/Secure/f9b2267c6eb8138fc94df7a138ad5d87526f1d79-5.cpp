static int ldapsrv_check_packet_size(
	struct ldapsrv_connection *conn,
	size_t size)
{
	bool is_anonymous = false;
	size_t max_size = 0;

	max_size = lpcfg_ldap_max_anonymous_request_size(conn->lp_ctx);
	if (size <= max_size) {
		return LDAP_SUCCESS;
	}

	/*
	 * Request is larger than the maximum unauthenticated request size.
	 * As this code is called frequently we avoid calling
	 * security_token_is_anonymous if possible
	 */
	if (conn->session_info != NULL &&
		conn->session_info->security_token != NULL) {
		is_anonymous = security_token_is_anonymous(
			conn->session_info->security_token);
	}

	if (is_anonymous) {
		DBG_WARNING(
			"LDAP request size (%zu) exceeds (%zu)\n",
			size,
			max_size);
		return LDAP_UNWILLING_TO_PERFORM;
	}

	max_size = lpcfg_ldap_max_authenticated_request_size(conn->lp_ctx);
	if (size > max_size) {
		DBG_WARNING(
			"LDAP request size (%zu) exceeds (%zu)\n",
			size,
			max_size);
		return LDAP_UNWILLING_TO_PERFORM;
	}
	return LDAP_SUCCESS;

}
static apr_byte_t oidc_is_back_channel_logout(const char *logout_param_value) {
	return ((logout_param_value != NULL) && (apr_strnatcmp(logout_param_value,
			OIDC_BACKCHANNEL_STYLE_LOGOUT_PARAM_VALUE) == 0));
}
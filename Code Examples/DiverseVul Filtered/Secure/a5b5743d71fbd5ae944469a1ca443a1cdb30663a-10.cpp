static unsigned int php_sapi_filter_init(TSRMLS_D)
{
	IF_G(get_array) = NULL;
	IF_G(post_array) = NULL;
	IF_G(cookie_array) = NULL;
	IF_G(server_array) = NULL;
	IF_G(env_array) = NULL;
	IF_G(session_array) = NULL;
	return SUCCESS;
}
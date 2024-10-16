static void php_filter_init_globals(zend_filter_globals *filter_globals) /* {{{ */
{
	filter_globals->post_array = NULL;
	filter_globals->get_array = NULL;
	filter_globals->cookie_array = NULL;
	filter_globals->env_array = NULL;
	filter_globals->server_array = NULL;
	filter_globals->session_array = NULL;
	filter_globals->default_filter = FILTER_DEFAULT;
}
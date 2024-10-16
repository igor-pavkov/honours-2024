PHP_RSHUTDOWN_FUNCTION(filter)
{
	VAR_ARRAY_COPY_DTOR(get_array)
	VAR_ARRAY_COPY_DTOR(post_array)
	VAR_ARRAY_COPY_DTOR(cookie_array)
	VAR_ARRAY_COPY_DTOR(server_array)
	VAR_ARRAY_COPY_DTOR(env_array)
	VAR_ARRAY_COPY_DTOR(session_array)
	return SUCCESS;
}
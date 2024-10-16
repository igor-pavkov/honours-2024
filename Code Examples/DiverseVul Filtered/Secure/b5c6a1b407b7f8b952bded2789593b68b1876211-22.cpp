static bool credtype_match(git_http_auth_scheme *scheme, void *data)
{
	unsigned int credtype = *(unsigned int *)data;

	return !!(scheme->credtypes & credtype);
}
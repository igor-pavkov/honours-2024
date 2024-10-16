static bool get_user_list(struct torture_context *torture, char ***users)
{
	struct winbindd_request req;
	struct winbindd_response rep;
	char **u = NULL;
	uint32_t count;
	char name[256];
	const char *extra_data;

	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);

	DO_STRUCT_REQ_REP(WINBINDD_LIST_USERS, &req, &rep);

	extra_data = (char *)rep.extra_data.data;
	torture_assert(torture, extra_data, "NULL extra data");

	for(count = 0;
	    next_token(&extra_data, name, ",", sizeof(name));
	    count++)
	{
		u = talloc_realloc(torture, u, char *, count + 2);
		u[count+1] = NULL;
		u[count] = talloc_strdup(u, name);
	}

	SAFE_FREE(rep.extra_data.data);

	*users = u;
	return true;
}
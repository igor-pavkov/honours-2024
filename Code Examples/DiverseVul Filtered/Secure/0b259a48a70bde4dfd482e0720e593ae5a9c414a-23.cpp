static bool get_group_list(struct torture_context *torture,
			   unsigned int *num_entries,
			   char ***groups)
{
	struct winbindd_request req;
	struct winbindd_response rep;
	char **g = NULL;
	uint32_t count;
	char name[256];
	const char *extra_data;

	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);

	DO_STRUCT_REQ_REP(WINBINDD_LIST_GROUPS, &req, &rep);
	extra_data = (char *)rep.extra_data.data;

	*num_entries = rep.data.num_entries;

	if (*num_entries == 0) {
		torture_assert(torture, extra_data == NULL,
			       "extra data is null for >0 reported entries\n");
		*groups = NULL;
		return true;
	}

	torture_assert(torture, extra_data, "NULL extra data");

	for(count = 0;
	    next_token(&extra_data, name, ",", sizeof(name));
	    count++)
	{
		g = talloc_realloc(torture, g, char *, count + 2);
		g[count+1] = NULL;
		g[count] = talloc_strdup(g, name);
	}

	SAFE_FREE(rep.extra_data.data);

	torture_assert_int_equal(torture, *num_entries, count,
				 "Wrong number of group entries reported.");

	*groups = g;
	return true;
}
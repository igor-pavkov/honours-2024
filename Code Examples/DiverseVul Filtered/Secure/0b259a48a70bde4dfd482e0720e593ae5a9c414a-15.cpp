static bool torture_winbind_struct_lookup_name_sid(struct torture_context *torture)
{
	struct winbindd_request req;
	struct winbindd_response rep;
	const char *invalid_sid = "S-0-0-7";
	char *domain = NULL;
	const char *invalid_user = "noone";
	char *invalid_name;
	bool strict = torture_setting_bool(torture, "strict mode", false);
	char **users;
	char **groups;
	uint32_t count, num_groups;
	bool ok;

	torture_comment(torture, "Running WINBINDD_LOOKUP_NAME_SID (struct based)\n");

	ok = get_user_list(torture, &users);
	torture_assert(torture, ok, "failed to retrieve list of users");
	lookup_name_sid_list(torture, users);

	ok = get_group_list(torture, &num_groups, &groups);
	torture_assert(torture, ok, "failed to retrieve list of groups");
	if (num_groups > 0) {
		lookup_name_sid_list(torture, groups);
	}

	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);

	fstrcpy(req.data.sid, invalid_sid);

	ok = true;
	DO_STRUCT_REQ_REP_EXT(WINBINDD_LOOKUPSID, &req, &rep,
			      NSS_STATUS_NOTFOUND,
			      strict,
			      ok=false,
			      talloc_asprintf(torture,
					      "invalid sid %s was resolved",
					      invalid_sid));

	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);

	/* try to find an invalid name... */

	count = 0;
	get_winbind_domain(torture, &domain);
	do {
		count++;
		invalid_name = talloc_asprintf(torture, "%s/%s%u",
					       domain,
					       invalid_user, count);
	} while(name_is_in_list(invalid_name, users) ||
		name_is_in_list(invalid_name, groups));

	fstrcpy(req.data.name.dom_name, domain);
	fstrcpy(req.data.name.name,
		talloc_asprintf(torture, "%s%u", invalid_user,
				count));

	ok = true;
	DO_STRUCT_REQ_REP_EXT(WINBINDD_LOOKUPNAME, &req, &rep,
			      NSS_STATUS_NOTFOUND,
			      strict,
			      ok=false,
			      talloc_asprintf(torture,
					      "invalid name %s was resolved",
					      invalid_name));

	talloc_free(users);
	talloc_free(groups);

	return true;
}
static bool lookup_name_sid_list(struct torture_context *torture, char **list)
{
	uint32_t count;

	for (count = 0; list[count]; count++) {
		struct winbindd_request req;
		struct winbindd_response rep;
		char *sid;
		char *name;
		const char *domain_name = torture_setting_string(torture,
						"winbindd_domain_without_prefix",
						NULL);

		ZERO_STRUCT(req);
		ZERO_STRUCT(rep);

		parse_domain_user(torture, list[count], req.data.name.dom_name,
				  req.data.name.name);

		DO_STRUCT_REQ_REP(WINBINDD_LOOKUPNAME, &req, &rep);

		sid = talloc_strdup(torture, rep.data.sid.sid);

		ZERO_STRUCT(req);
		ZERO_STRUCT(rep);

		fstrcpy(req.data.sid, sid);

		DO_STRUCT_REQ_REP(WINBINDD_LOOKUPSID, &req, &rep);

		if (domain_name != NULL &&
		    strequal(rep.data.name.dom_name, domain_name))
		{
			name = talloc_asprintf(torture, "%s",
					       rep.data.name.name);
		} else {
			name = talloc_asprintf(torture, "%s%c%s",
					       rep.data.name.dom_name,
					       winbind_separator(torture),
					       rep.data.name.name);
		}

		torture_assert_casestr_equal(torture, list[count], name,
					 "LOOKUP_SID after LOOKUP_NAME != id");

#if 0
		torture_comment(torture, " %s -> %s -> %s\n", list[count],
				sid, name);
#endif

		talloc_free(sid);
		talloc_free(name);
	}

	return true;
}
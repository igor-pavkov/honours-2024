static bool torture_winbind_struct_list_trustdom(struct torture_context *torture)
{
	struct winbindd_request req;
	struct winbindd_response rep;
	char *list1;
	char *list2;
	bool ok;
	struct torture_trust_domain *listd = NULL;
	uint32_t i;

	torture_comment(torture, "Running WINBINDD_LIST_TRUSTDOM (struct based)\n");

	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);

	req.data.list_all_domains = false;

	DO_STRUCT_REQ_REP(WINBINDD_LIST_TRUSTDOM, &req, &rep);

	list1 = (char *)rep.extra_data.data;

	torture_comment(torture, "%s\n", list1);

	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);

	req.data.list_all_domains = true;

	DO_STRUCT_REQ_REP(WINBINDD_LIST_TRUSTDOM, &req, &rep);

	list2 = (char *)rep.extra_data.data;

	/*
	 * The list_all_domains parameter should be ignored
	 */
	torture_assert_str_equal(torture, list2, list1, "list_all_domains not ignored");

	SAFE_FREE(list1);
	SAFE_FREE(list2);

	ok = get_trusted_domains(torture, &listd);
	torture_assert(torture, ok, "failed to get trust list");

	for (i=0; listd && listd[i].netbios_name; i++) {
		if (i == 0) {
			struct dom_sid *builtin_sid;

			builtin_sid = dom_sid_parse_talloc(torture, SID_BUILTIN);

			torture_assert_str_equal(torture,
						 listd[i].netbios_name,
						 NAME_BUILTIN,
						 "first domain should be 'BUILTIN'");

			torture_assert_str_equal(torture,
						 listd[i].dns_name,
						 "",
						 "BUILTIN domain should not have a dns name");

			ok = dom_sid_equal(builtin_sid,
					   listd[i].sid);
			torture_assert(torture, ok, "BUILTIN domain should have S-1-5-32");

			continue;
		}

		/*
		 * TODO: verify the content of the 2nd and 3rd (in member server mode)
		 *       domain entries
		 */
	}

	return true;
}
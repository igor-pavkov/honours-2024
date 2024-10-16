static bool get_trusted_domains(struct torture_context *torture,
				struct torture_trust_domain **_d)
{
	struct winbindd_request req;
	struct winbindd_response rep;
	struct torture_trust_domain *d = NULL;
	uint32_t dcount = 0;
	char line[256];
	const char *extra_data;

	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);

	DO_STRUCT_REQ_REP(WINBINDD_LIST_TRUSTDOM, &req, &rep);

	extra_data = (char *)rep.extra_data.data;
	torture_assert(torture, extra_data != NULL,
		       "Trust list was NULL: the list of trusted domain "
		       "should be returned, with at least 2 entries "
		       "(BUILTIN, and the local domain)");

	while (next_token(&extra_data, line, "\n", sizeof(line))) {
		char *p, *lp;

		d = talloc_realloc(torture, d,
				   struct torture_trust_domain,
				   dcount + 2);
		ZERO_STRUCT(d[dcount+1]);

		lp = line;
		p = strchr(lp, '\\');
		torture_assert(torture, p, "missing 1st '\\' in line");
		*p = 0;
		d[dcount].netbios_name = talloc_strdup(d, lp);
		torture_assert(torture, strlen(d[dcount].netbios_name) > 0,
			       "empty netbios_name");

		lp = p+1;
		p = strchr(lp, '\\');
		torture_assert(torture, p, "missing 2nd '\\' in line");
		*p = 0;
		d[dcount].dns_name = talloc_strdup(d, lp);
		/* it's ok to have an empty dns_name */

		lp = p+1;
		d[dcount].sid = dom_sid_parse_talloc(d, lp);
		torture_assert(torture, d[dcount].sid,
			       "failed to parse sid");

		dcount++;
	}
	SAFE_FREE(rep.extra_data.data);

	torture_assert(torture, dcount >= 2,
		       "The list of trusted domain should contain 2 entries "
		       "(BUILTIN, and the local domain)");

	*_d = d;
	return true;
}
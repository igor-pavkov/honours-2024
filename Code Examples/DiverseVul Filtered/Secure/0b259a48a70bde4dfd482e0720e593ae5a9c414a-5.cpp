static bool torture_winbind_struct_domain_info(struct torture_context *torture)
{
	bool ok;
	struct torture_trust_domain *listd = NULL;
	uint32_t i;

	torture_comment(torture, "Running WINBINDD_DOMAIN_INFO (struct based)\n");

	ok = get_trusted_domains(torture, &listd);
	torture_assert(torture, ok, "failed to get trust list");

	for (i=0; listd && listd[i].netbios_name; i++) {
		torture_comment(torture, "LIST[%u] '%s' => '%s' [%s]\n",
				(unsigned)i,
				listd[i].netbios_name,
				listd[i].dns_name,
				dom_sid_string(torture, listd[i].sid));
	}

	for (i=0; listd && listd[i].netbios_name; i++) {
		struct winbindd_request req;
		struct winbindd_response rep;
		struct dom_sid *sid;
		char *flagstr = talloc_strdup(torture," ");

		ZERO_STRUCT(req);
		ZERO_STRUCT(rep);

		fstrcpy(req.domain_name, listd[i].netbios_name);

		DO_STRUCT_REQ_REP(WINBINDD_DOMAIN_INFO, &req, &rep);

		if (rep.data.domain_info.primary) {
			flagstr = talloc_strdup_append(flagstr, "PR ");
		}

		if (rep.data.domain_info.active_directory) {
			torture_assert(torture,
				       strlen(rep.data.domain_info.alt_name)>0,
				       "Active Directory without DNS name");
			flagstr = talloc_strdup_append(flagstr, "AD ");
		}

		if (rep.data.domain_info.native_mode) {
			torture_assert(torture,
				       rep.data.domain_info.active_directory,
				       "Native-Mode, but no Active Directory");
			flagstr = talloc_strdup_append(flagstr, "NA ");
		}

		torture_comment(torture, "DOMAIN[%u] '%s' => '%s' [%s] [%s]\n",
				(unsigned)i,
				rep.data.domain_info.name,
				rep.data.domain_info.alt_name,
				flagstr,
				rep.data.domain_info.sid);

		sid = dom_sid_parse_talloc(torture, rep.data.domain_info.sid);
		torture_assert(torture, sid, "Failed to parse SID");

		ok = dom_sid_equal(listd[i].sid, sid);
		torture_assert(torture, ok, talloc_asprintf(torture, "SID's doesn't match [%s] != [%s]",
			       dom_sid_string(torture, listd[i].sid),
			       dom_sid_string(torture, sid)));

		torture_assert_str_equal(torture,
					 rep.data.domain_info.name,
					 listd[i].netbios_name,
					 "Netbios domain name doesn't match");

		torture_assert_str_equal(torture,
					 rep.data.domain_info.alt_name,
					 listd[i].dns_name,
					 "DNS domain name doesn't match");
	}

	return true;
}
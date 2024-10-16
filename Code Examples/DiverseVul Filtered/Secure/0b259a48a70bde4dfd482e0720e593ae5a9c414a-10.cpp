static bool torture_winbind_struct_getdcname(struct torture_context *torture)
{
	bool ok;
	bool strict = torture_setting_bool(torture, "strict mode", false);
	const char *domain_name = torture_setting_string(torture,
					"winbindd_netbios_domain",
					lpcfg_workgroup(torture->lp_ctx));
	struct torture_trust_domain *listd = NULL;
	uint32_t i, count = 0;

	torture_comment(torture, "Running WINBINDD_GETDCNAME (struct based)\n");

	ok = get_trusted_domains(torture, &listd);
	torture_assert(torture, ok, "failed to get trust list");

	for (i=0; listd && listd[i].netbios_name; i++) {
		struct winbindd_request req;
		struct winbindd_response rep;

		/* getdcname is not expected to work on "BUILTIN" or our own
		 * domain */
		if (strequal(listd[i].netbios_name, "BUILTIN") ||
		    strequal(listd[i].netbios_name, domain_name)) {
			continue;
		}

		ZERO_STRUCT(req);
		ZERO_STRUCT(rep);

		fstrcpy(req.domain_name, listd[i].netbios_name);

		ok = true;
		DO_STRUCT_REQ_REP_EXT(WINBINDD_GETDCNAME, &req, &rep,
				      NSS_STATUS_SUCCESS,
				      (i <2 || strict), ok = false,
				      talloc_asprintf(torture, "DOMAIN '%s'",
				      		      req.domain_name));
		if (!ok) continue;

		/* TODO: check rep.data.dc_name; */
		torture_comment(torture, "DOMAIN '%s' => DCNAME '%s'\n",
				req.domain_name, rep.data.dc_name);
		count++;
	}

	if (strict) {
		torture_assert(torture, count > 0,
			       "WiNBINDD_GETDCNAME was not tested");
	}
	return true;
}
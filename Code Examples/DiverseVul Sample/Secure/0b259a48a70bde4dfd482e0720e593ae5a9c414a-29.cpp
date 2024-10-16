static bool torture_winbind_struct_dsgetdcname(struct torture_context *torture)
{
	bool ok;
	bool strict = torture_setting_bool(torture, "strict mode", false);
	struct torture_trust_domain *listd = NULL;
	uint32_t i;
	uint32_t count = 0;

	torture_comment(torture, "Running WINBINDD_DSGETDCNAME (struct based)\n");

	ok = get_trusted_domains(torture, &listd);
	torture_assert(torture, ok, "failed to get trust list");

	for (i=0; listd && listd[i].netbios_name; i++) {
		struct winbindd_request req;
		struct winbindd_response rep;

		ZERO_STRUCT(req);
		ZERO_STRUCT(rep);

		if (strlen(listd[i].dns_name) == 0) continue;

		/*
		 * TODO: remove this and let winbindd give no dns name
		 *       for NT4 domains
		 */
		if (strcmp(listd[i].dns_name, listd[i].netbios_name) == 0) {
			continue;
		}

		fstrcpy(req.domain_name, listd[i].dns_name);

		/* TODO: test more flag combinations */
		req.flags = DS_DIRECTORY_SERVICE_REQUIRED;

		ok = true;
		DO_STRUCT_REQ_REP_EXT(WINBINDD_DSGETDCNAME, &req, &rep,
				      NSS_STATUS_SUCCESS,
				      strict, ok = false,
				      talloc_asprintf(torture, "DOMAIN '%s'",
						      req.domain_name));
		if (!ok) continue;

		/* TODO: check rep.data.dc_name; */
		torture_comment(torture, "DOMAIN '%s' => DCNAME '%s'\n",
				req.domain_name, rep.data.dc_name);

		count++;
	}

	if (count == 0) {
		torture_warning(torture, "WINBINDD_DSGETDCNAME"
				" was not tested with %d non-AD domains",
				i);
	}

	if (strict) {
		torture_assert(torture, count > 0,
			       "WiNBINDD_DSGETDCNAME was not tested");
	}

	return true;
}
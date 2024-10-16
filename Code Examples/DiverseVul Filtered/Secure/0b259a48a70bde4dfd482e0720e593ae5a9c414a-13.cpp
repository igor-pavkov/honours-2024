static bool torture_winbind_struct_show_sequence(struct torture_context *torture)
{
	bool ok;
	uint32_t i;
	struct torture_trust_domain *domlist = NULL;
	struct torture_domain_sequence *s = NULL;

	torture_comment(torture, "Running WINBINDD_SHOW_SEQUENCE (struct based)\n");

	ok = get_sequence_numbers(torture, &s);
	torture_assert(torture, ok, "failed to get list of sequence numbers");

	ok = get_trusted_domains(torture, &domlist);
	torture_assert(torture, ok, "failed to get trust list");

	for (i=0; domlist[i].netbios_name; i++) {
		struct winbindd_request req;
		struct winbindd_response rep;
		uint32_t seq;

		torture_assert(torture, s[i].netbios_name,
			       "more domains received in second run");
		torture_assert_str_equal(torture, domlist[i].netbios_name,
					 s[i].netbios_name,
					 "inconsistent order of domain lists");

		ZERO_STRUCT(req);
		ZERO_STRUCT(rep);
		fstrcpy(req.domain_name, domlist[i].netbios_name);

		ok = true;
		DO_STRUCT_REQ_REP_EXT(WINBINDD_SHOW_SEQUENCE, &req, &rep,
				      NSS_STATUS_SUCCESS,
				      false, ok = false,
				      "WINBINDD_SHOW_SEQUENCE");
		if (ok == false) {
			torture_warning(torture,
					"WINBINDD_SHOW_SEQUENCE on "
					"domain %s failed\n",
					req.domain_name);

			/*
			 * Only fail for the first two domain that we
			 * check specially below, otherwise we fail on
			 * trusts generated by the LSA torture test
			 * that do not really exist.
			 */
			if (i > 1) {
				/*
				 * Do not confirm the sequence numbers
				 * below
				 */
				return true;
			}

			torture_comment(torture,
					"Full trust list for "
					"WINBINDD_SHOW_SEQUENCE "
					"test was:\n");
			for (i=0; domlist[i].netbios_name; i++) {
				torture_comment(torture,
						"%s\n",
						domlist[i].netbios_name);
			}

			return false;
		}

		seq = rep.data.sequence_number;

		if (i == 0) {
			torture_assert(torture, (seq != (uint32_t)-1),
				       "BUILTIN domain disconnected");
		} else if (i == 1) {
			torture_assert(torture, (seq != (uint32_t)-1),
				       "local domain disconnected");
		}


		if (seq == (uint32_t)-1) {
			torture_comment(torture, " * %s : DISCONNECTED\n",
					req.domain_name);
		} else {
			torture_comment(torture, " * %s : %d\n",
					req.domain_name, seq);
		}
		torture_assert(torture, (seq >= s[i].seq),
			       "illegal sequence number encountered");
	}

	return true;
}
static bool get_sequence_numbers(struct torture_context *torture,
				 struct torture_domain_sequence **seqs)
{
	struct winbindd_request req;
	struct winbindd_response rep;
	const char *extra_data;
	char line[256];
	uint32_t count = 0;
	struct torture_domain_sequence *s = NULL;

	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);

	DO_STRUCT_REQ_REP(WINBINDD_SHOW_SEQUENCE, &req, &rep);

	extra_data = (char *)rep.extra_data.data;
	torture_assert(torture, extra_data, "NULL sequence list");

	while (next_token(&extra_data, line, "\n", sizeof(line))) {
		char *p, *lp;
		uint32_t seq;

		s = talloc_realloc(torture, s, struct torture_domain_sequence,
				   count + 2);
		ZERO_STRUCT(s[count+1]);

		lp = line;
		p = strchr(lp, ' ');
		torture_assert(torture, p, "invalid line format");
		*p = 0;
		s[count].netbios_name = talloc_strdup(s, lp);

		lp = p+1;
		torture_assert(torture, strncmp(lp, ": ", 2) == 0,
			       "invalid line format");
		lp += 2;
		if (strcmp(lp, "DISCONNECTED") == 0) {
			seq = (uint32_t)-1;
		} else {
			seq = (uint32_t)strtol(lp, &p, 10);
			torture_assert(torture, (*p == '\0'),
				       "invalid line format");
			torture_assert(torture, (seq != (uint32_t)-1),
				       "sequence number -1 encountered");
		}
		s[count].seq = seq;

		count++;
	}
	SAFE_FREE(rep.extra_data.data);

	torture_assert(torture, count >= 2, "The list of domain sequence "
		       "numbers should contain 2 entries");

	*seqs = s;
	return true;
}
static bool get_winbind_domain(struct torture_context *torture, char **domain)
{
	struct winbindd_response rep;

	ZERO_STRUCT(rep);

	DO_STRUCT_REQ_REP(WINBINDD_DOMAIN_NAME, NULL, &rep);

	*domain = talloc_strdup(torture, rep.data.domain_name);
	torture_assert(torture, domain, "talloc error");

	return true;
}
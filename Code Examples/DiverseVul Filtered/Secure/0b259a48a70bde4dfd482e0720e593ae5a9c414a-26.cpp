static bool torture_winbind_struct_endpwent(struct torture_context *torture)
{
	struct winbindd_request req;
	struct winbindd_response rep;

	torture_comment(torture, "Running WINBINDD_ENDPWENT (struct based)\n");

	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);

	DO_STRUCT_REQ_REP(WINBINDD_ENDPWENT, &req, &rep);

	return true;
}
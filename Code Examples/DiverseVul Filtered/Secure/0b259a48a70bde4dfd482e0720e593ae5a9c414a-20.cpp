static char winbind_separator(struct torture_context *torture)
{
	struct winbindd_response rep;

	ZERO_STRUCT(rep);

	DO_STRUCT_REQ_REP(WINBINDD_INFO, NULL, &rep);

	return rep.data.info.winbind_separator;
}
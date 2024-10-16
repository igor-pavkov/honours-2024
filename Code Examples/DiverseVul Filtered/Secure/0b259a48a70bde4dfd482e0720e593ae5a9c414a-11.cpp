static bool torture_winbind_struct_priv_pipe_dir(struct torture_context *torture)
{
	struct winbindd_response rep;
	const char *got_dir;

	ZERO_STRUCT(rep);

	torture_comment(torture, "Running WINBINDD_PRIV_PIPE_DIR (struct based)\n");

	DO_STRUCT_REQ_REP(WINBINDD_PRIV_PIPE_DIR, NULL, &rep);

	got_dir = (const char *)rep.extra_data.data;

	torture_assert(torture, got_dir, "NULL WINBINDD_PRIV_PIPE_DIR\n");

	SAFE_FREE(rep.extra_data.data);
	return true;
}
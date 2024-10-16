static bool torture_winbind_struct_info(struct torture_context *torture)
{
	struct winbindd_response rep;
	const char *separator;

	ZERO_STRUCT(rep);

	torture_comment(torture, "Running WINBINDD_INFO (struct based)\n");

	DO_STRUCT_REQ_REP(WINBINDD_INFO, NULL, &rep);

	separator = torture_setting_string(torture,
					   "winbindd_separator",
					   lpcfg_winbind_separator(torture->lp_ctx));

	torture_assert_int_equal(torture,
				 rep.data.info.winbind_separator,
				 *separator,
				 "winbind separator doesn't match");

	torture_comment(torture, "Samba Version '%s'\n",
			rep.data.info.samba_version);

	return true;
}
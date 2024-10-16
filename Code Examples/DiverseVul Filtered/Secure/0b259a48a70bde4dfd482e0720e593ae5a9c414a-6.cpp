static bool torture_winbind_struct_netbios_name(struct torture_context *torture)
{
	struct winbindd_response rep;
	const char *expected;

	ZERO_STRUCT(rep);

	torture_comment(torture, "Running WINBINDD_NETBIOS_NAME (struct based)\n");

	DO_STRUCT_REQ_REP(WINBINDD_NETBIOS_NAME, NULL, &rep);

	expected = torture_setting_string(torture,
					  "winbindd_netbios_name",
					  lpcfg_netbios_name(torture->lp_ctx));
	expected = strupper_talloc(torture, expected);

	torture_assert_str_equal(torture,
				 rep.data.netbios_name, expected,
				 "winbindd's netbios name doesn't match");

	return true;
}
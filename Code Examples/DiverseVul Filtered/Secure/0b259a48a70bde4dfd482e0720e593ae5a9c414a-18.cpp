static bool torture_winbind_struct_domain_name(struct torture_context *torture)
{
	const char *expected;
	char *domain;

	torture_comment(torture, "Running WINBINDD_DOMAIN_NAME (struct based)\n");

	expected = torture_setting_string(torture,
					  "winbindd_netbios_domain",
					  lpcfg_workgroup(torture->lp_ctx));

	get_winbind_domain(torture, &domain);

	torture_assert_str_equal(torture, domain, expected,
				 "winbindd's netbios domain doesn't match");

	return true;
}
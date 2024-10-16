static bool torture_winbind_struct_check_machacc(struct torture_context *torture)
{
	bool ok;
	bool strict = torture_setting_bool(torture, "strict mode", false);
	struct winbindd_response rep;

	ZERO_STRUCT(rep);

	torture_comment(torture, "Running WINBINDD_CHECK_MACHACC (struct based)\n");

	ok = true;
	DO_STRUCT_REQ_REP_EXT(WINBINDD_CHECK_MACHACC, NULL, &rep,
			      NSS_STATUS_SUCCESS, strict, ok = false,
			      "WINBINDD_CHECK_MACHACC");

	if (!ok) {
		torture_assert(torture,
			       strlen(rep.data.auth.nt_status_string)>0,
			       "Failed with empty nt_status_string");

		torture_warning(torture,"%s:%s:%s:%d\n",
				nt_errstr(NT_STATUS(rep.data.auth.nt_status)),
				rep.data.auth.nt_status_string,
				rep.data.auth.error_string,
				rep.data.auth.pam_error);
		return true;
	}

	torture_assert_ntstatus_ok(torture,
				   NT_STATUS(rep.data.auth.nt_status),
				   "WINBINDD_CHECK_MACHACC ok: nt_status");

	torture_assert_str_equal(torture,
				 rep.data.auth.nt_status_string,
				 nt_errstr(NT_STATUS_OK),
				 "WINBINDD_CHECK_MACHACC ok:nt_status_string");

	torture_assert_str_equal(torture,
				 rep.data.auth.error_string,
				 get_friendly_nt_error_msg(NT_STATUS_OK),
				 "WINBINDD_CHECK_MACHACC ok: error_string");

	torture_assert_int_equal(torture,
				 rep.data.auth.pam_error,
				 nt_status_to_pam(NT_STATUS_OK),
				 "WINBINDD_CHECK_MACHACC ok: pam_error");

	return true;
}
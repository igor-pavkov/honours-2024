static bool torture_winbind_struct_getpwent(struct torture_context *torture)
{
	struct winbindd_request req;
	struct winbindd_response rep;
	struct winbindd_pw *pwent;

	torture_comment(torture, "Running WINBINDD_GETPWENT (struct based)\n");

	torture_comment(torture, " - Running WINBINDD_SETPWENT first\n");
	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);
	DO_STRUCT_REQ_REP(WINBINDD_SETPWENT, &req, &rep);

	torture_comment(torture, " - Running WINBINDD_GETPWENT now\n");
	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);
	req.data.num_entries = 1;
	if (torture_setting_bool(torture, "samba3", false)) {
		DO_STRUCT_REQ_REP_EXT(WINBINDD_GETPWENT, &req, &rep,
				      NSS_STATUS_SUCCESS, false, _STRUCT_NOOP,
				      NULL);
	} else {
		DO_STRUCT_REQ_REP(WINBINDD_GETPWENT, &req, &rep);
	}
	pwent = (struct winbindd_pw *)rep.extra_data.data;
	if (!torture_setting_bool(torture, "samba3", false)) {
		torture_assert(torture, (pwent != NULL), "NULL pwent");
	}
	if (pwent) {
		torture_comment(torture, "name: %s, uid: %d, gid: %d, shell: %s\n",
				pwent->pw_name, pwent->pw_uid, pwent->pw_gid,
				pwent->pw_shell);
	}

	return true;
}
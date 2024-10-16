static bool torture_winbind_struct_lookup_sids_invalid(
	struct torture_context *torture)
{
	struct winbindd_request req = {0};
	struct winbindd_response rep = {0};
	bool strict = torture_setting_bool(torture, "strict mode", false);
	bool ok;

	torture_comment(torture,
			"Running WINBINDD_LOOKUP_SIDS (struct based)\n");

	ok = true;
	DO_STRUCT_REQ_REP_EXT(WINBINDD_LOOKUPSIDS, &req, &rep,
			      NSS_STATUS_NOTFOUND,
			      strict,
			      ok=false,
			      talloc_asprintf(
				      torture,
				      "invalid lookupsids succeeded"));

	return ok;
}
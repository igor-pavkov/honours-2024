static bool torture_winbind_struct_interface_version(struct torture_context *torture)
{
	struct winbindd_request req;
	struct winbindd_response rep;

	ZERO_STRUCT(req);
	ZERO_STRUCT(rep);

	torture_comment(torture, "Running WINBINDD_INTERFACE_VERSION (struct based)\n");

	DO_STRUCT_REQ_REP(WINBINDD_INTERFACE_VERSION, &req, &rep);

	torture_assert_int_equal(torture,
				 rep.data.interface_version,
				 WINBIND_INTERFACE_VERSION,
				 "winbind server and client doesn't match");

	return true;
}
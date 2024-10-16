IRC_Analyzer::IRC_Analyzer(Connection* conn)
: tcp::TCP_ApplicationAnalyzer("IRC", conn)
	{
	invalid_msg_count = 0;
	invalid_msg_max_count = 20;
	orig_status = WAIT_FOR_REGISTRATION;
	resp_status = WAIT_FOR_REGISTRATION;
	orig_zip_status = NO_ZIP;
	resp_zip_status = NO_ZIP;
	starttls = false;
	cl_orig = new tcp::ContentLine_Analyzer(conn, true, 1000);
	AddSupportAnalyzer(cl_orig);
	cl_resp = new tcp::ContentLine_Analyzer(conn, false, 1000);
	AddSupportAnalyzer(cl_resp);
	}
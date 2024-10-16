static int hidp_session_dev_init(struct hidp_session *session,
				 struct hidp_connadd_req *req)
{
	int ret;

	if (req->rd_size > 0) {
		ret = hidp_setup_hid(session, req);
		if (ret && ret != -ENODEV)
			return ret;
	}

	if (!session->hid) {
		ret = hidp_setup_input(session, req);
		if (ret < 0)
			return ret;
	}

	return 0;
}
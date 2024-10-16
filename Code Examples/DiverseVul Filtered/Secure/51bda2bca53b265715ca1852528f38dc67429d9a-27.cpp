static void hidp_session_dev_del(struct hidp_session *session)
{
	if (session->hid)
		hid_destroy_device(session->hid);
	else if (session->input)
		input_unregister_device(session->input);
}
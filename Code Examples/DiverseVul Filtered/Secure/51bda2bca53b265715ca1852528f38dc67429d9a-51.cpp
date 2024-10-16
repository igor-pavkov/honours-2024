static void hidp_session_dev_destroy(struct hidp_session *session)
{
	if (session->hid)
		put_device(&session->hid->dev);
	else if (session->input)
		input_put_device(session->input);

	kfree(session->rd_data);
	session->rd_data = NULL;
}
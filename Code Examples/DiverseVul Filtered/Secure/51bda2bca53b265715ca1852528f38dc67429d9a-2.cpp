static void hidp_session_dev_work(struct work_struct *work)
{
	struct hidp_session *session = container_of(work,
						    struct hidp_session,
						    dev_init);
	int ret;

	ret = hidp_session_dev_add(session);
	if (!ret)
		atomic_inc(&session->state);
	else
		hidp_session_terminate(session);
}
static void hidp_idle_timeout(unsigned long arg)
{
	struct hidp_session *session = (struct hidp_session *) arg;

	hidp_session_terminate(session);
}
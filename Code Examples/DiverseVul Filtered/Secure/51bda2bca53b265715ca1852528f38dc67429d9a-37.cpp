static struct hidp_session *hidp_session_find(const bdaddr_t *bdaddr)
{
	struct hidp_session *session;

	down_read(&hidp_session_sem);

	session = __hidp_session_find(bdaddr);
	if (session)
		hidp_session_get(session);

	up_read(&hidp_session_sem);

	return session;
}
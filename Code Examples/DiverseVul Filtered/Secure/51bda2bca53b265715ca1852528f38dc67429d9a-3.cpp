int hidp_get_conninfo(struct hidp_conninfo *ci)
{
	struct hidp_session *session;

	session = hidp_session_find(&ci->bdaddr);
	if (session) {
		hidp_copy_session(session, ci);
		hidp_session_put(session);
	}

	return session ? 0 : -ENOENT;
}
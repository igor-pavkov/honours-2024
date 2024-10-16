static struct hidp_session *__hidp_session_find(const bdaddr_t *bdaddr)
{
	struct hidp_session *session;

	list_for_each_entry(session, &hidp_session_list, list) {
		if (!bacmp(bdaddr, &session->bdaddr))
			return session;
	}

	return NULL;
}
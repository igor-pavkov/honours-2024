static struct hidp_session *__hidp_get_session(bdaddr_t *bdaddr)
{
	struct hidp_session *session;

	BT_DBG("");

	list_for_each_entry(session, &hidp_session_list, list) {
		if (!bacmp(bdaddr, &session->bdaddr))
			return session;
	}

	return NULL;
}
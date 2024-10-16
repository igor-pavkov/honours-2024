static void hidp_session_get(struct hidp_session *session)
{
	kref_get(&session->ref);
}
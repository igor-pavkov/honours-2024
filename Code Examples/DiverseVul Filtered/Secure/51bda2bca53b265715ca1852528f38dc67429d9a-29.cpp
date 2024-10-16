static void hidp_session_put(struct hidp_session *session)
{
	kref_put(&session->ref, session_free);
}
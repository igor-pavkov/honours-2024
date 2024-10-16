static void hidp_session_remove(struct l2cap_conn *conn,
				struct l2cap_user *user)
{
	struct hidp_session *session = container_of(user,
						    struct hidp_session,
						    user);

	down_write(&hidp_session_sem);

	hidp_session_terminate(session);

	cancel_work_sync(&session->dev_init);
	if (session->input ||
	    atomic_read(&session->state) > HIDP_SESSION_PREPARING)
		hidp_session_dev_del(session);

	list_del(&session->list);

	up_write(&hidp_session_sem);

	hidp_session_put(session);
}
static void session_free(struct kref *ref)
{
	struct hidp_session *session = container_of(ref, struct hidp_session,
						    ref);

	hidp_session_dev_destroy(session);
	skb_queue_purge(&session->ctrl_transmit);
	skb_queue_purge(&session->intr_transmit);
	fput(session->intr_sock->file);
	fput(session->ctrl_sock->file);
	l2cap_conn_put(session->conn);
	kfree(session);
}
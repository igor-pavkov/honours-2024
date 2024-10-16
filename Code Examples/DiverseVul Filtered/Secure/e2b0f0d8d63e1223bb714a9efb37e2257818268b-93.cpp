static void session_init_browsing(struct avrcp *session)
{
	if (session->browsing_timer > 0) {
		timeout_remove(session->browsing_timer);
		session->browsing_timer = 0;
	}

	session->browsing_id = avctp_register_browsing_pdu_handler(
							session->conn,
							handle_browsing_pdu,
							session,
							destroy_browsing);
}